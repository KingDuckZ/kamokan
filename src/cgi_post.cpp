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

#include "cgi_post.hpp"
#include "cgi_env.hpp"
#include "split_get_vars.hpp"
#include "curl_wrapper.hpp"
#include <iostream>
#include <iterator>
#include <algorithm>
#include <string>
#include <cassert>
#include <ciso646>

namespace tawashi {
	namespace cgi {
		namespace {
		} //unnamed namespace

		const PostMapType& read_post (const Env& parEnv) {
			static bool already_read = false;
			static PostMapType map;
			static std::string original_data;

			if (not already_read) {
				assert(original_data.empty());
				assert(map.empty());

				const auto input_len = parEnv.content_length();
				if (input_len > 0) {
					original_data.reserve(input_len);
					std::copy_n(
						std::istream_iterator<char>(std::cin),
						input_len,
						std::back_inserter(original_data)
					);

					CurlWrapper curl;
					for (auto& itm : split_env_vars(original_data)) {
						map[unescape_string(curl, itm.first)] = unescape_string(curl, itm.second);
					}
				}

				already_read = true;
			}

			return map;
		}
	} //namespace cgi
} //namespace tawashi
