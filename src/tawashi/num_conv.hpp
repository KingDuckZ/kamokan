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

#include <ciso646>
#include <duckhandy/lexical_cast.hpp>

namespace tawashi {
	template <typename T>
	inline bool seems_valid_number (const boost::string_ref& parValue) {
		const std::size_t skip_sign = (sprout::is_signed<T>::value and not parValue.empty() and parValue[0] == '-' ? 1 : 0);
		for (std::size_t z = skip_sign; z < parValue.size(); ++z) {
			const char c = parValue[z];
			if (c < '0' or c > '9')
				return false;
		}
		return parValue.size() - skip_sign <= dhandy::tags::dec<T>::count_digits_bt(sprout::numeric_limits<T>::max());
	}
} //namespace tawashi
