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

#include "sanitized_utf8.hpp"
#include "utf8.h"
#include <iterator>

namespace tawashi {
	std::string sanitized_utf8 (const boost::string_ref& parStr) {
		std::string sanitized;
		sanitized.reserve(parStr.size());
		utf8::replace_invalid(parStr.begin(), parStr.end(), std::back_inserter(sanitized));
		return sanitized;
	}
} //namespace tawashi
