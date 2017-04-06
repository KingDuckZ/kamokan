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

#include "cgi_environment_vars.hpp"
#include "get_env.hpp"

namespace tawashi {
	std::vector<std::string> cgi_environment_vars() {
		using boost::string_ref;

		std::vector<std::string> retlist;
		retlist.reserve(CGIVars::_size());

		for (CGIVars var : CGIVars::_values()) {
			auto value = get_env_as<string_ref>(var._to_string(), "");
			retlist.push_back(std::string(value.data(), value.size()));
		}
		return retlist;
	}
} //namespace tawashi
