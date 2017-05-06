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

#include "fake_cgi_env.hpp"
#include "duckhandy/lexical_cast.hpp"
#include <cassert>

namespace tawashi {
	namespace cgi {
		FakeEnv::FakeEnv (std::string&& parVariablesIni) :
			m_variables(std::move(parVariablesIni)),
			m_auth_type(m_variables.parsed().at("fake_env").at("auth_type")),
			m_content_type(m_variables.parsed().at("fake_env").at("content_type")),
			m_path_info(m_variables.parsed().at("fake_env").at("path_info")),
			m_path_translated(m_variables.parsed().at("fake_env").at("path_translated")),
			m_query_string(m_variables.parsed().at("fake_env").at("query_string")),
			m_remote_addr(m_variables.parsed().at("fake_env").at("remote_addr")),
			m_remote_host(m_variables.parsed().at("fake_env").at("remote_host")),
			m_remote_ident(m_variables.parsed().at("fake_env").at("remote_ident")),
			m_remote_user(m_variables.parsed().at("fake_env").at("remote_user")),
			m_request_method(m_variables.parsed().at("fake_env").at("request_method")),
			m_script_name(m_variables.parsed().at("fake_env").at("script_name")),
			m_server_name(m_variables.parsed().at("fake_env").at("server_name")),
			m_server_software(m_variables.parsed().at("fake_env").at("server_software")),
			m_content_length(dhandy::lexical_cast<std::size_t>(m_variables.parsed().at("fake_env").at("content_length"))),
			m_server_port(dhandy::lexical_cast<uint16_t>(m_variables.parsed().at("fake_env").at("server_port")))
		{
		}

		const std::string& FakeEnv::auth_type() const {
			return m_auth_type;
		}
		std::size_t FakeEnv::content_length() const {
			std::size_t m_content_length;
		}
		const std::string& FakeEnv::content_type() const {
			return m_content_type;
		}
		auto FakeEnv::gateway_interface() const -> boost::optional<VersionInfo> {
		}
		const std::string& FakeEnv::path_info() const {
			return m_path_info;
		}
		const std::string& FakeEnv::path_translated() const {
			return m_path_translated;
		}
		const std::string& FakeEnv::query_string() const {
			return m_query_string;
		}
		const std::string& FakeEnv::remote_addr() const {
			return m_remote_addr;
		}
		const std::string& FakeEnv::remote_host() const {
			return m_remote_host;
		}
		const std::string& FakeEnv::remote_ident() const {
			return m_remote_ident;
		}
		const std::string& FakeEnv::remote_user() const {
			return m_remote_user;
		}
		const std::string& FakeEnv::request_method() const {
			return m_request_method;
		}
		const std::string& FakeEnv::script_name() const {
			return m_script_name;
		}
		const std::string& FakeEnv::server_name() const {
			return m_server_name;
		}
		uint16_t FakeEnv::server_port() const {
			uint16_t m_server_port;
		}
		auto FakeEnv::server_protocol() const -> boost::optional<VersionInfo> {
		}
		const std::string& FakeEnv::server_software() const {
			return m_server_software;
		}
		auto FakeEnv::query_string_split() const -> GetMapType {
		}
	} //namespace cgi
} //namespace tawashi
