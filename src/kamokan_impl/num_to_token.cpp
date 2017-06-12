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

#include "num_to_token.hpp"
#include <cassert>

namespace kamokan {
	namespace {
		//const int g_any_min = 0;
		//const int g_any_max = g_any_min + 'z' - 'a' - 1;
		//const int g_vowel_min = g_any_max + 1;
		//const int g_vowel_max = g_vowel_min + 5 - 1;
		//const int g_consonant_min = g_vowel_max + 1;
		//const int g_consonant_max = g_consonant_min + ('z' - 'a') - (g_vowel_max - g_vowel_min) - 1;

		//char code_to_char (int parCode) {
		//	const char vowels[] = {'a', 'i', 'u', 'e', 'o'};
		//	const char consonants[] = {
		//		'b', 'c', 'd', 'f', 'g', 'h', 'j', 'k', 'l', 'm', 'n', 'p',
		//		'q', 'r', 's', 't', 'v', 'w', 'x', 'y', 'z'
		//	};

		//	static_assert(sizeof(vowels) == g_vowel_max - g_vowel_min + 1, "Wrong vowels count");
		//	static_assert(sizeof(consonants) == g_consonant_max - g_consonant_min + 1, "Wrong consonants count");

		//	if (parCode <= g_any_max)
		//		return static_cast<char>('a' + parCode - g_any_min);
		//	else if (parCode <= g_vowel_max)
		//		return vowels[parCode - g_vowel_min];
		//	else if (parCode <= g_consonant_max)
		//		return consonants[parCode - g_consonant_min];

		//	assert(false);
		//	return 'X';
		//}
	} //unnamed namespace

	std::string num_to_token (int64_t parNum) {
		assert(0 < parNum);
		std::string retval;

		do {
			const auto remainder = parNum % ('z' - 'a' + 1);
			retval.push_back(static_cast<char>('a' + remainder));
			parNum /= ('z' - 'a' + 1);
		} while (parNum);

		return retval;
	}
} //namespace kamokan
