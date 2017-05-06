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

#pragma once

#include "cgi_env.hpp"
#include "ini_file.hpp"
#include <string>

namespace tawashi {
	namespace cgi {
		class FakeEnv : public Env {
		public:
			explicit FakeEnv (std::string&& parVariablesIni);

			virtual const std::string& auth_type() const override;
			virtual std::size_t content_length() const override;
			virtual const std::string& content_type() const override;
			virtual boost::optional<VersionInfo> gateway_interface() const override a_pure;
			virtual const std::string& path_info() const override;
			virtual const std::string& path_translated() const override;
			virtual const std::string& query_string() const override;
			virtual const std::string& remote_addr() const override;
			virtual const std::string& remote_host() const override;
			virtual const std::string& remote_ident() const override;
			virtual const std::string& remote_user() const override;
			virtual const std::string& request_method() const override;
			virtual const std::string& script_name() const override;
			virtual const std::string& server_name() const override;
			virtual uint16_t server_port() const override a_pure;
			virtual boost::optional<VersionInfo> server_protocol() const override a_pure;
			virtual const std::string& server_software() const override;

			virtual GetMapType query_string_split() const override a_pure;

		private:
			IniFile m_variables;
			std::string m_auth_type;
			std::string m_content_type;
			std::string m_path_info;
			std::string m_path_translated;
			std::string m_query_string;
			std::string m_remote_addr;
			std::string m_remote_host;
			std::string m_remote_ident;
			std::string m_remote_user;
			std::string m_request_method;
			std::string m_script_name;
			std::string m_server_name;
			std::string m_server_software;
			std::size_t m_content_length;
			uint16_t m_server_port;
		};
	} //namespace cgi
} //namespace tawashi
