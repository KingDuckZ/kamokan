/* Copyright 2017, Michele Santullo
 * This file is part of "kamokan".
 *
 * "kamokan" is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * "kamokan" is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with "kamokan".  If not, see <http://www.gnu.org/licenses/>.
 */

#include "response.hpp"
#include "settings_bag.hpp"
#include "duckhandy/stringize.h"
#include "duckhandy/lexical_cast.hpp"
#include "pathname/pathname.hpp"
#include "list_highlight_langs.hpp"
#include "cgi_env.hpp"
#include "num_conv.hpp"
#include "kamokan_config.h"
#include "version.hpp"
#include <utility>
#include <cassert>
#include <fstream>
#include <sstream>
#include <functional>
#include <boost/optional.hpp>
#include <cstdint>
#include <spdlog/spdlog.h>

namespace kamokan {
	namespace {
		//boost::string_ref fetch_page_basename (const cgi::Env& parEnv) {
		//	const boost::string_ref& path = parEnv.path_info();

		//	const std::size_t last_slash = path.rfind('/');
		//	const std::size_t last_dot = path.rfind('.');
		//	const std::size_t start_index = (path.npos == last_slash ? 0 : last_slash + 1);
		//	const std::size_t substr_len = (path.size() - start_index - (last_dot == path.npos ? 0 : path.size() - last_dot));
		//	assert(start_index <= path.size());
		//	assert(substr_len < path.size() and substr_len - path.size() - start_index);
		//	return path.substr(start_index, substr_len);
		//}

		std::string make_root_path (const SettingsBag& parSettings) {
			auto retval = parSettings["website_root"];
			if (retval.empty()) {
				return "";
			}
			else {
				return mchlib::PathName(retval).path() + '/';
			}
		}

		boost::optional<std::string> load_whole_file (const std::string& parWebsiteRoot, const char* parSuffix, const boost::string_view& parName, bool parThrow) {
			std::ostringstream oss;
			oss << parWebsiteRoot << parName << parSuffix;
			spdlog::get("statuslog")->info("Trying to load \"{}\"", oss.str());
			std::ifstream if_mstch(oss.str(), std::ios::binary | std::ios::in);

			if (not if_mstch) {
				spdlog::get("statuslog")->warn("Couldn't open file \"{}\"", oss.str());
				if (parThrow)
					throw std::runtime_error(std::string("File \"") + oss.str() + "\" not found");
				else
					return boost::optional<std::string>();
			}

			std::ostringstream buffer;
			buffer << if_mstch.rdbuf();
			return boost::make_optional(buffer.str());
		}

		mstch::array make_mstch_langmap (const SettingsBag& parSettings) {
			mstch::array retval;

			for (auto&& lang : list_highlight_langs(parSettings)) {
				retval.push_back(mstch::map{{"language_name", std::move(lang)}});
			}
			return retval;
		}

		std::string disable_mstch_escaping (const std::string& parStr) {
			return parStr;
		};

		boost::string_view make_host_path (const SettingsBag& parSettings) {
			boost::string_view host_path = parSettings.at("host_path");
			if (not host_path.empty() and host_path[host_path.size() - 1] == '/')
				host_path = host_path.substr(0, host_path.size() - 1);
			return host_path;
		}

		std::string make_base_uri (const Kakoune::SafePtr<SettingsBag>& parSettings, const Kakoune::SafePtr<cgi::Env>& parCgiEnv) {
			assert(parSettings);
			assert(parCgiEnv);

			std::ostringstream oss;
			if (parCgiEnv->https())
				oss << "https://";
			else
				oss << "http://";
			oss << parSettings->at("host_name");
			boost::string_view host_port = parSettings->at("host_port");
			if (not host_port.empty()) {
				if (host_port == "from_downstream") {
					const uint16_t port = parCgiEnv->server_port();
					if ((80 != port and not parCgiEnv->https()) or (443 != port and parCgiEnv->https())) {
						oss << ':' << port;
					}
				}
				else if (not host_port.empty() and tawashi::seems_valid_number<uint16_t>(host_port)) {
					oss << ':' << host_port;
				}
			}

			oss << make_host_path(*parSettings);
			return oss.str();
		}
	} //unnamed namespace

	Response::Response (
		const Kakoune::SafePtr<SettingsBag>& parSettings,
		std::ostream* parStreamOut,
		const Kakoune::SafePtr<cgi::Env>& parCgiEnv,
		bool parWantRedis
	) :
		m_storage(parSettings),
		//m_page_basename(fetch_page_basename(m_cgi_env)),
		m_cgi_env(parCgiEnv),
		m_settings(parSettings),
		m_time0(std::chrono::steady_clock::now()),
		m_website_root(make_root_path(*parSettings)),
		m_base_uri(make_base_uri(m_settings, m_cgi_env)),
		m_stream_out(parStreamOut)
	{
		assert(m_cgi_env);
		assert(m_stream_out);

		if (parWantRedis) {
			m_storage.connect_async();
		}

		mstch::config::escape = &disable_mstch_escaping;

		auto statuslog = spdlog::get("statuslog");
		assert(statuslog);
		statuslog->info("Preparing response for {} request; query_string=\"{}\"; size={}",
			cgi_env().request_method()._to_string(),
			cgi_env().query_string(),
			cgi_env().content_length()
		);
	}

	Response::~Response() noexcept = default;

	tawashi::HttpHeader Response::on_process() {
		return tawashi::HttpHeader();
	}

	void Response::on_mustache_prepare (mstch::map&) {
	}

	void Response::send() {
		auto statuslog = spdlog::get("statuslog");
		assert(statuslog);

		statuslog->info("Sending response");
		SPDLOG_TRACE(statuslog, "Preparing mustache dictionary");
		mstch::map mustache_context {
			{"submit_page", this->is_submit_page()},
			{"version", boost::string_view{STRINGIZE(VERSION_MAJOR) "." STRINGIZE(VERSION_MINOR) "." STRINGIZE(VERSION_PATCH)}},
			{"tawashi_version", tawashi::version()},
			{"base_uri", base_uri()},
			{"host_path", make_host_path(this->settings())},
			{"languages", make_mstch_langmap(*m_settings)}
		};

		m_storage.finalize_connection();

		SPDLOG_TRACE(statuslog, "Raising event on_process");
		tawashi::HttpHeader http_header = this->on_process();
		*m_stream_out << http_header;

		if (http_header.body_required()) {
			using std::chrono::steady_clock;
			using std::chrono::milliseconds;
			using std::chrono::duration_cast;

			SPDLOG_TRACE(statuslog, "Raising event on_mustache_prepare");
			this->on_mustache_prepare(mustache_context);

			const auto time1 = steady_clock::now();
			const int page_time = duration_cast<milliseconds>(time1 - m_time0).count();
			mustache_context["page_time"] = page_time;
			SPDLOG_TRACE(statuslog, "Rendering in mustache");
			*m_stream_out << mstch::render(
				on_mustache_retrieve(),
				mustache_context,
				std::bind(
					&load_whole_file,
					std::cref(m_website_root),
					".mustache",
					std::placeholders::_1,
					false
				)
			);
		}

		SPDLOG_TRACE(statuslog, "Flushing output");
		m_stream_out->flush();
	}

	std::string Response::on_mustache_retrieve() {
		return load_mustache();
	}

	const cgi::Env& Response::cgi_env() const {
		return *m_cgi_env;
	}

	const cgi::PostMapType& Response::cgi_post() const {
		return cgi::read_post(std::cin, cgi_env(), settings().as<uint32_t>("max_post_size"));
	}

	const std::string& Response::base_uri() const {
		return m_base_uri;
	}

	std::string Response::load_mustache() const {
		boost::optional<std::string> content = load_whole_file(m_website_root, ".html.mstch", page_basename(), true);
		return *content;
	}

	const Storage& Response::storage() const {
		assert(m_storage.is_connected());
		return m_storage;
	}

	const SettingsBag& Response::settings() const {
		assert(m_settings);
		return *m_settings;
	}

	tawashi::HttpHeader Response::make_redirect (tawashi::HttpStatusCodes parCode, const std::string& parLocation) {
		std::ostringstream oss;
		oss << base_uri() << '/' << parLocation;

		auto statuslog = spdlog::get("statuslog");
		assert(statuslog);
		statuslog->info("Redirecting to page \"{}\"", oss.str());
		return tawashi::HttpHeader(parCode, oss.str());
	}

	tawashi::HttpHeader Response::make_error_redirect (tawashi::ErrorReasons parReason) {
		using tawashi::HttpStatusCodes;

		auto statuslog = spdlog::get("statuslog");
		assert(statuslog);
		const HttpStatusCodes redir_code = HttpStatusCodes::Code302_Found;
		statuslog->info("Redirecting to error page, code={} reason={}", redir_code, parReason);

		std::ostringstream oss;
		oss << "error.cgi?reason=" << parReason._to_integral();
		return make_redirect(redir_code, oss.str());
	}

	void Response::set_app_start_time (const std::chrono::time_point<std::chrono::steady_clock>& parTime) {
		assert(parTime <= m_time0);
		m_time0 = parTime;
	}
} //namespace kamokan
