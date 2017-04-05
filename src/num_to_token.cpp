#include "num_to_token.hpp"
#include <cassert>

namespace tawashi {
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

	std::string make_token (uint64_t parNum) {
		assert(0 != parNum);
		std::string retval;

		do {
			const auto remainder = parNum % ('z' - 'a' + 1);
			retval.push_back(static_cast<char>(remainder));
			parNum /= ('z' - 'a' + 1);
		} while (parNum);

		return retval;
	}
} //namespace tawashi
