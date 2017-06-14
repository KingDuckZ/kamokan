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

#include "string_conv.hpp"
#include <sstream>

namespace kamokan {
	template <>
	bool string_conv (boost::string_view parStr) {
		if (parStr == "true" or parStr == "yes" or parStr == "1" or parStr == "on") {
			return true;
		}
		else if (parStr == "false" or parStr == "no" or parStr == "0" or parStr == "off") {
			return false;
		}
		else {
			std::ostringstream oss;
			oss << "Bad conversion: can't convert \"" << parStr << "\" to bool";
			throw std::runtime_error(oss.str());
		}
	}

	template bool string_conv<bool> (boost::string_view parStr);
} //namespace kamokan
