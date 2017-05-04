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

#include <boost/utility/string_ref.hpp>
#include <string>
#if !defined(_GNU_SOURCE)
#	define _GNU_SOURCE
#endif
#include <cstddef>
#include <utility>
#include <boost/optional.hpp>

namespace tawashi {
	boost::optional<std::string> get_env (const char* parName);

	template <typename A>
	A get_env_as (const char* parName, const A& parDefault);

	template <>
	std::string get_env_as (const char* parName, const std::string& parDefault);
	template <>
	std::size_t get_env_as (const char* parName, const std::size_t& parDefault);
} //namespace tawashi
