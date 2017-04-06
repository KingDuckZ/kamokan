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

#include "get_env.hpp"
#include "duckhandy/lexical_cast.hpp"
#include <cstdlib>

namespace tawashi {
	boost::optional<boost::string_ref> get_env (const char* parName) {
		using boost::string_ref;
		using boost::make_optional;
		using boost::optional;

		const char* const raw_getvar = secure_getenv(parName);
		return (raw_getvar ? make_optional(string_ref(raw_getvar)) : optional<string_ref>());
	}

	template <>
	std::string get_env_as (const char* parName, const std::string& parDefault) {
		auto var = get_env(parName);
		return (var ? std::string(var->data(), var->size()) : parDefault);
	}

	template <>
	boost::string_ref get_env_as (const char* parName, const boost::string_ref& parDefault) {
		auto var = get_env(parName);
		return (var ? *var : parDefault);
	}

	template <>
	std::size_t get_env_as (const char* parName, const std::size_t& parDefault) {
		using dhandy::lexical_cast;
		auto var = get_env(parName);
		return (var ? lexical_cast<std::size_t>(*var) : parDefault);
	}
} //namespace tawashi
