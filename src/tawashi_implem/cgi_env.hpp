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

#include "split_get_vars.hpp"
#include "duckhandy/compatibility.h"
#include "escapist.hpp"
#include "kakoune/safe_ptr.hh"
#include "tawashiConfig.h"
#include <vector>
#include <string>
#include <boost/utility/string_ref.hpp>
#include <cstdint>
#include <iostream>
#include <boost/optional.hpp>
#include <boost/container/flat_map.hpp>

namespace tawashi {
	namespace cgi {
		class Env : public Kakoune::SafeCountable {
		public:
			struct VersionInfo {
				boost::string_ref name;
				uint16_t major;
				uint16_t minor;
			};

			typedef boost::container::flat_map<std::string, std::string> GetMapType;

			explicit Env (const char* const* parEnvList);
			virtual_testing ~Env() noexcept;

			virtual_testing const std::string& auth_type() const;
			virtual_testing std::size_t content_length() const;
			virtual_testing const std::string& content_type() const;
			virtual_testing boost::optional<VersionInfo> gateway_interface() const a_pure;
			virtual_testing const std::string& path_info() const;
			virtual_testing const std::string& path_translated() const;
			virtual_testing const std::string& query_string() const;
			virtual_testing const std::string& remote_addr() const;
			virtual_testing const std::string& remote_host() const;
			virtual_testing const std::string& remote_ident() const;
			virtual_testing const std::string& remote_user() const;
			virtual_testing const std::string& request_method() const;
			virtual_testing const std::string& script_name() const;
			virtual_testing const std::string& server_name() const;
			virtual_testing uint16_t server_port() const a_pure;
			virtual_testing boost::optional<VersionInfo> server_protocol() const a_pure;
			virtual_testing const std::string& server_software() const;

			virtual_testing GetMapType query_string_split() const a_pure;

			std::ostream& print_all (std::ostream& parStream, const char* parNewline) const;

		private:
			std::vector<std::string> m_cgi_env;
			Escapist m_houdini;
		};
	} //namespace cgi
} //namespace tawashi
