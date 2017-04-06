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
