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
#include <iterator>

#define SANITIZE_WITH_UTFCPP

#if defined(SANITIZE_WITH_UTFCPP)
#	include "utf8.h"
#	include <algorithm>
#else
#	include <glib.h>
#	include <cassert>
#	include <array>
#endif

namespace tawashi {
	std::string sanitized_utf8 (const boost::string_view& parStr) {
		std::string sanitized;
		sanitized.reserve(parStr.size());
#if defined(SANITIZE_WITH_UTFCPP)
		utf8::replace_invalid(parStr.begin(), parStr.end(), std::back_inserter(sanitized), 0xFFFD);
		std::replace(sanitized.begin(), sanitized.end(), '\0', '#');
#else
#	error "untested code, don't enable in final builds"
		std::array<char, 6> replacement;
		const int replacement_len = g_unichar_to_utf8(0xFFFD, replacement.data());

		std::size_t beg_offset = 0;
		const char* end;
		while (not g_utf8_validate(parStr.data() + beg_offset, parStr.size() - beg_offset, &end)) {
			assert(beg_offset < parStr.size());
			const std::size_t valid_chunk_size = end - (parStr.data() + beg_offset);
			sanitized.append(parStr.data() + beg_offset, end);
			if (*end)
				sanitized.append(replacement.data(), replacement_len);
			else
				sanitized.append({ '#' });
			beg_offset += valid_chunk_size + 1;
			assert(beg_offset <= parStr.size());
		}
		sanitized.append(parStr.data() + beg_offset, end);
		assert(g_utf8_validate(sanitized.data(), sanitized.size(), nullptr));
#endif
		return sanitized;
	}
} //namespace tawashi
