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
#include "escapist.hpp"
#include "sanitized_utf8.hpp"
#include "spdlog.hpp"
#if defined(SPDLOG_DEBUG_ON)
#	include "truncated_string.hpp"
#endif
#include <iostream>
#include <iterator>
#include <algorithm>
#include <string>
#include <cassert>
#include <ciso646>

namespace tawashi {
	UnsupportedContentTypeException::UnsupportedContentTypeException (const boost::string_view& parMessage) :
		Exception(ErrorReasons::UnsupportedContentType, parMessage)
	{
	}

	namespace cgi {
		namespace {
			bool valid_content_type (const Env& parEnv) {
				if (parEnv.content_type_split().type != "application" or
					parEnv.content_type_split().subtype !=
						"x-www-form-urlencoded") {
					return false;
				}
				return true;
			}

			std::string read_n (std::istream& parSrc, std::size_t parSize) {
				if (0 == parSize)
					return std::string();

				std::string original_data;
				original_data.reserve(parSize);
				std::copy_n(
					std::istream_iterator<char>(parSrc),
					parSize,
					std::back_inserter(original_data)
				);
				return sanitized_utf8(original_data);
			}
		} //unnamed namespace

		const PostMapType& read_post (std::istream& parSrc, const Env& parEnv) {
			return read_post(parSrc, parEnv, parEnv.content_length());
		}

		const PostMapType& read_post (std::istream& parSrc, const Env& parEnv, std::size_t parMaxLen) {
			static bool already_read = false;
			static PostMapType map;
			static std::string original_data;

			if (not already_read) {
				assert(original_data.empty());
				assert(map.empty());

				if (not valid_content_type(parEnv)) {
					throw UnsupportedContentTypeException(parEnv.content_type());
				}

				const auto input_len = std::min(parMaxLen, parEnv.content_length());
				original_data = read_n(parSrc, input_len);
				Escapist houdini;
				for (auto& itm : split_env_vars(original_data)) {
#if defined(SPDLOG_DEBUG_ON)
					if (itm.first == "pastie") {
						spdlog::get("statuslog")->debug("Raw pastie from POST, size={}: \"{}\"", itm.second.size(), truncated_string(itm.second, 30));
					}
#endif
					std::string key(houdini.unescape_url(itm.first));
					std::string val(houdini.unescape_url(itm.second));
					map[std::move(key)] = std::move(val);
				}

				already_read = true;
			}

			return map;
		}
	} //namespace cgi
} //namespace tawashi
