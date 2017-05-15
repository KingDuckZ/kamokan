/* Copyright 2017, Michele Santullo
 * This file is part of "tawashi".
 *
 * "tawashi" is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * "tawashi" is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with "tawashi".  If not, see <http://www.gnu.org/licenses/>.
 */

#include "response.hpp"
#include "incredis/incredis.hpp"
#include "settings_bag.hpp"
#include "tawashiConfig.h"
#include "duckhandy/stringize.h"
#include "pathname/pathname.hpp"
#include "list_highlight_langs.hpp"
#include "cgi_env.hpp"
#include <utility>
#include <cassert>
#include <fstream>
#include <sstream>
#include <functional>
#include <boost/optional.hpp>
#include <cstdint>
#include <spdlog/spdlog.h>

namespace tawashi {
	namespace {
		const char g_def_response_type[] = "text/html";

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

		redis::IncRedis make_incredis (const tawashi::SettingsBag& parSettings) {
			using redis::IncRedis;

			if (parSettings["redis_mode"] == "inet") {
				return IncRedis(
					parSettings.as<std::string>("redis_server"),
					parSettings.as<uint16_t>("redis_port")
				);
			}
			else if (parSettings["redis_mode"] == "sock") {
				return IncRedis(parSettings.as<std::string>("redis_sock"));
			}
			else {
				throw std::runtime_error("Unknown setting for \"redis_mode\", valid settings are \"inet\" or \"sock\"");
			}
		}

		boost::optional<std::string> load_whole_file (const std::string& parWebsiteRoot, const char* parSuffix, const boost::string_ref& parName, bool parThrow) {
			std::ostringstream oss;
			oss << parWebsiteRoot << parName << parSuffix;
			spdlog::get("statuslog")->info("Trying to load \"{}\"", oss.str());
			std::ifstream if_mstch(oss.str(), std::ios::binary | std::ios::in);

			if (not if_mstch) {
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
	} //unnamed namespace

	Response::Response (
		const Kakoune::SafePtr<SettingsBag>& parSettings,
		std::ostream* parStreamOut,
		const Kakoune::SafePtr<cgi::Env>& parCgiEnv,
		bool parWantRedis
	) :
		m_resp_value(g_def_response_type),
		//m_page_basename(fetch_page_basename(m_cgi_env)),
		m_cgi_env(parCgiEnv),
		m_settings(parSettings),
		m_website_root(make_root_path(*parSettings)),
		m_resp_type(ContentType),
		m_stream_out(parStreamOut),
		m_header_sent(false)
	{
		assert(m_cgi_env);
		assert(m_stream_out);

		if (parWantRedis) {
			m_redis = std::make_unique<redis::IncRedis>(make_incredis(*parSettings));
			m_redis->connect();
		}

		mstch::config::escape = &disable_mstch_escaping;

		auto statuslog = spdlog::get("statuslog");
		assert(statuslog);
		statuslog->info("Preparing Response for {} request to {}; size: {}",
			cgi_env().request_method(),
			cgi_env().query_string(),
			cgi_env().content_length()
		);
	}

	Response::~Response() noexcept = default;

	void Response::on_process() {
	}

	void Response::on_mustache_prepare (mstch::map&) {
	}

	void Response::send() {
		auto statuslog = spdlog::get("statuslog");
		assert(statuslog);

		statuslog->info("Sending response");
		SPDLOG_TRACE(statuslog, "Preparing mustache dictionary");
		mstch::map mustache_context {
			{"version", std::string{STRINGIZE(VERSION_MAJOR) "." STRINGIZE(VERSION_MINOR) "." STRINGIZE(VERSION_PATCH)}},
			{"base_uri", m_settings->as<std::string>("base_uri")},
			{"languages", make_mstch_langmap(*m_settings)}
		};

		if (m_redis) {
			SPDLOG_TRACE(statuslog, "Finalizing redis connection");
			m_redis->wait_for_connect();
			auto batch = m_redis->make_batch();
			batch.select(m_settings->as<uint32_t>("redis_db"));
			batch.client_setname("tawashi_v" STRINGIZE(VERSION_MAJOR) "." STRINGIZE(VERSION_MINOR) "." STRINGIZE(VERSION_PATCH));
			batch.throw_if_failed();
		}

		SPDLOG_TRACE(statuslog, "Raising event on_process");
		this->on_process();
		SPDLOG_TRACE(statuslog, "Raising event on_mustache_prepare");
		this->on_mustache_prepare(mustache_context);

		m_header_sent = true;
		switch (m_resp_type) {
		case ContentType:
			SPDLOG_TRACE(statuslog, "Response is a Content-type (data)");
			*m_stream_out << "Content-type: " << m_resp_value << "\n\n";
			break;
		case Location:
			SPDLOG_TRACE(statuslog, "Response is a Location (redirect)");
			*m_stream_out << "Location: " << m_resp_value << "\n\n";
			break;
		}

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
		SPDLOG_TRACE(statuslog, "Flushing output");
		m_stream_out->flush();
	}

	std::string Response::on_mustache_retrieve() {
		return load_mustache();
	}

	const cgi::Env& Response::cgi_env() const {
		return *m_cgi_env;
	}

	void Response::change_type (Types parRespType, std::string&& parValue) {
		assert(not m_header_sent);
		assert(not parValue.empty());
		m_resp_type = parRespType;
		m_resp_value = std::move(parValue);
	}

	const boost::string_ref& Response::base_uri() const {
		return m_settings->at("base_uri");
	}

	std::string Response::load_mustache() const {
		boost::optional<std::string> content = load_whole_file(m_website_root, ".html.mstch", page_basename(), true);
		return *content;
	}

	redis::IncRedis& Response::redis() const {
		assert(m_redis);
		return *m_redis;
	}

	const SettingsBag& Response::settings() const {
		assert(m_settings);
		return *m_settings;
	}
} //namespace tawashi
