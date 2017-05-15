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

#include "cgi_environment_vars.hpp"
#include "sanitized_utf8.hpp"
#include "string_lengths.hpp"
#include <utility>
#include <unordered_map>
#include <boost/utility/string_ref.hpp>
#include <cassert>
#include <cstring>
#include <boost/functional/hash.hpp>
#include <cstddef>
#if !defined(NDEBUG)
#	include <cstring>
#endif

namespace std {
	template<>
	struct hash<boost::string_ref> {
		std::size_t operator() (boost::string_ref const& parStr) const {
			return boost::hash_range(parStr.begin(), parStr.end());
		}
	};
} //namespace std

namespace tawashi {
	namespace {
		std::unordered_map<boost::string_ref, boost::string_ref> get_unrefined_env_vars (const char* const* parEnvList) {
			using boost::string_ref;

			assert(parEnvList);
			std::size_t count = 0;
			while (*(parEnvList + count)) {
				++count;
			}

			std::unordered_map<string_ref, string_ref> retval;
			retval.reserve(count);
			for (std::size_t z = 0; z < count; ++z) {
				const char* const equal_sign = std::strchr(parEnvList[z], '=');
				assert('=' == *equal_sign);
				assert(equal_sign >= parEnvList[z]);
				const std::size_t key_length = static_cast<std::size_t>(equal_sign - parEnvList[z]);
				const std::size_t whole_length = std::strlen(parEnvList[z] + key_length) + key_length;
				assert(std::strlen(parEnvList[z]) == whole_length);
				assert(whole_length >= key_length + 1);
				retval[string_ref(parEnvList[z], key_length)] = string_ref(parEnvList[z] + key_length + 1, whole_length - key_length - 1);
			}
			return retval;
		}
	} //unnamed namespace

	std::vector<std::string> cgi_environment_vars (const char* const* parEnvList) {
		using boost::string_ref;

		std::vector<std::string> retlist;
		retlist.reserve(CGIVars::_size());

		auto unrefined_env_vars = get_unrefined_env_vars(parEnvList);
		auto enum_str_lengths = string_lengths<CGIVars>();

		std::size_t z = 0;
		for (CGIVars var : CGIVars::_values()) {
#if !defined(NDEBUG)
			assert(std::strlen(var._to_string()) == enum_str_lengths[z]);
#endif
			auto it_found = unrefined_env_vars.find(boost::string_ref(var._to_string(), enum_str_lengths[z]));
			if (unrefined_env_vars.cend() != it_found)
				retlist.push_back(sanitized_utf8(it_found->second));
			else
				retlist.push_back(std::string());
			++z;
		}
		return retlist;
	}
} //namespace tawashi
