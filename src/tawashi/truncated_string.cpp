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

#include "truncated_string.hpp"
#include <cassert>
#include <algorithm>
#include <ciso646>

namespace tawashi {
	std::string truncated_string (const boost::string_view& parString, std::size_t parMaxLen) {
		const std::size_t char_length = std::min(parMaxLen, parString.size());
		std::string retval;
		retval.reserve(char_length);
		std::size_t doubled_up = 0;
		for (auto c : parString) {
			const std::size_t len = ('\r' == c or '\n' == c ? 2 : 1);
			const char replaced_chara = ('\r' == c ? 'r' : ('\n' == c ? 'n' : c));
			const char addition[2] = { '\\', replaced_chara };
			if (retval.size() + len > parMaxLen)
				break;

			retval.append(addition + (2 - len), len);
			doubled_up += (len - 1);
		}
		assert(
			retval.size()  == std::min(parString.size() + doubled_up, parMaxLen) or
			retval.size() + 1  == std::min(parString.size() + doubled_up, parMaxLen)
		);
		if (retval.size() - doubled_up < parString.size())
			retval += "...";

		return retval;
	}
} //namespace tawashi
