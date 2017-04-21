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
#include "ini_file.hpp"
#include "tawashiConfig.h"
#include "duckhandy/stringize.h"
#include "pathname/pathname.hpp"
#include "list_highlight_langs.hpp"
#include <utility>
#include <cassert>
#include <fstream>
#include <sstream>
#include <functional>
#include <boost/optional.hpp>

namespace tawashi {
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

		std::string make_root_path (const IniFile::KeyValueMapType& parSettings) {
			const auto it_found = parSettings.find("website_root");
			if (parSettings.end() == it_found) {
				return "";
			}
			else {
				mchlib::PathName retval(it_found->second);
				return retval.path() + '/';
			}
		}

		redis::IncRedis make_incredis (const tawashi::IniFile::KeyValueMapType& parSettings) {
			using redis::IncRedis;

			if (parSettings.at("redis_mode") == "inet") {
				return IncRedis(
					std::string(parSettings.at("redis_server")),
					dhandy::lexical_cast<uint16_t>(parSettings.at("redis_port"))
				);
			}
			else if (parSettings.at("redis_mode") == "sock") {
				return IncRedis(std::string(parSettings.at("redis_sock")));
			}
			else {
				throw std::runtime_error("Unknown setting for \"redis_mode\", valid settings are \"inet\" or \"sock\"");
			}
		}

		boost::optional<std::string> load_whole_file (const std::string& parWebsiteRoot, const char* parSuffix, const std::string& parName, bool parThrow) {
			std::ostringstream oss;
			oss << parWebsiteRoot << parName << parSuffix;
			std::cerr << "Trying to load \"" << oss.str() << "\"\n";
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

		mstch::array make_mstch_langmap() {
			mstch::array retval;

			for (auto&& lang : list_highlight_langs()) {
				retval.push_back(mstch::map{{"language_name", std::move(lang)}});
			}
			return retval;
		}
	} //unnamed namespace

	Response::Response (Types parRespType, std::string&& parValue, std::string&& parPageBaseName, const IniFile& parIni, bool parWantRedis) :
		m_resp_value(std::move(parValue)),
		m_base_uri(parIni.parsed().at("tawashi").at("base_uri")),
		//m_page_basename(fetch_page_basename(m_cgi_env)),
		m_website_root(make_root_path(parIni.parsed().at("tawashi"))),
		m_page_basename(std::move(parPageBaseName)),
		m_resp_type(parRespType),
		m_header_sent(false)
	{
		if (parWantRedis) {
			m_redis = std::make_unique<redis::IncRedis>(make_incredis(parIni.parsed().at("tawashi")));
			m_redis->connect();
		}
	}

	Response::~Response() noexcept = default;

	void Response::on_process() {
	}

	void Response::on_send (std::ostream& parStream) {
		parStream << load_mustache();
	}

	void Response::on_mustache_prepare (mstch::map&) {
	}

	void Response::send() {
		mstch::map mustache_context {
			{"version", std::string{STRINGIZE(VERSION_MAJOR) "." STRINGIZE(VERSION_MINOR) "." STRINGIZE(VERSION_PATCH)}},
			{"base_uri", std::string(m_base_uri.data(), m_base_uri.size())},
			{"languages", make_mstch_langmap()}
		};

		if (m_redis)
			m_redis->wait_for_connect();

		this->on_process();
		this->on_mustache_prepare(mustache_context);

		m_header_sent = true;
		switch (m_resp_type) {
		case ContentType:
			std::cout << "Content-type: " << m_resp_value << "\n\n";
			break;
		case Location:
			std::cout << "Location: " << m_resp_value << "\n\n";
			break;
		}

		std::ostringstream stream_out;
		if (ContentType == m_resp_type)
			this->on_send(stream_out);
		std::cout << mstch::render(
			stream_out.str(),
			mustache_context,
			std::bind(
				&load_whole_file,
				std::cref(m_website_root),
				".mustache",
				std::placeholders::_1,
				false
			)
		);
		std::cout.flush();
	}

	const cgi::Env& Response::cgi_env() const {
		return m_cgi_env;
	}

	void Response::change_type (Types parRespType, std::string&& parValue) {
		assert(not m_header_sent);
		assert(not parValue.empty());
		m_resp_type = parRespType;
		m_resp_value = std::move(parValue);
	}

	const boost::string_ref& Response::base_uri() const {
		return m_base_uri;
	}

	const std::string& Response::page_basename() const {
		return m_page_basename;
	}

	std::string Response::load_mustache() const {
		boost::optional<std::string> content = load_whole_file(m_website_root, ".html.mstch", page_basename(), true);
		return *content;
	}

	redis::IncRedis& Response::redis() const {
		assert(m_redis);
		return *m_redis;
	}
} //namespace tawashi
