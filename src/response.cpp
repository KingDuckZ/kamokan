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
#include <utility>
#include <cassert>
#include <fstream>
#include <sstream>

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
	} //unnamed namespace

	Response::Response (Types parRespType, std::string&& parValue, std::string&& parPageBaseName, const IniFile& parIni, bool parWantRedis) :
		m_resp_value(std::move(parValue)),
		m_base_uri(parIni.parsed().at("tawashi").at("base_uri")),
		//m_page_basename(fetch_page_basename(m_cgi_env)),
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

	void Response::send() {
		if (m_redis)
			m_redis->wait_for_connect();

		this->on_process();

		m_header_sent = true;
		switch (m_resp_type) {
		case ContentType:
			std::cout << "Content-type: " << m_resp_value << "\n\n";
			break;
		case Location:
			std::cout << "Location: " << m_resp_value << "\n\n";
			break;
		}

		if (ContentType == m_resp_type)
			this->on_send(std::cout);
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
		std::ostringstream oss;
		oss << "html/" << page_basename() << ".html.mstch";
		std::cerr << "Trying to load \"" << oss.str() << "\"\n";
		std::ifstream if_mstch(oss.str(), std::ios::binary | std::ios::in);

		if (!if_mstch)
			throw std::runtime_error(std::string("File \"") + oss.str() + "\" not found");

		std::ostringstream buffer;
		buffer << if_mstch.rdbuf();
		return buffer.str();
	}

	redis::IncRedis& Response::redis() const {
		assert(m_redis);
		return *m_redis;
	}
} //namespace tawashi
