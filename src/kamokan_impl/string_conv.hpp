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

#pragma once

#include "duckhandy/lexical_cast.hpp"
#include <boost/utility/string_view.hpp>
#include <type_traits>
#include <cstdint>
#include <string>

namespace kamokan {
	template <typename T>
	[[gnu::pure]]
	T string_conv (boost::string_view parStr);

	template <>
	[[gnu::pure]]
	bool string_conv (boost::string_view parStr);

	template <>
	[[gnu::always_inline]] inline
	boost::string_view string_conv (boost::string_view parStr) {
		return parStr;
	}

	template <>
	[[gnu::pure,gnu::always_inline]] inline
	uint16_t string_conv (boost::string_view parStr) {
		return dhandy::lexical_cast<uint16_t>(parStr);
	}

	template <>
	[[gnu::pure,gnu::always_inline]] inline
	uint32_t string_conv (boost::string_view parStr) {
		return dhandy::lexical_cast<uint32_t>(parStr);
	}

	template <>
	inline
	std::string string_conv (boost::string_view parStr) {
		return std::string(parStr.data(), parStr.size());
	}
} //namespace kamokan
