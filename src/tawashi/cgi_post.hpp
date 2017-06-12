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

#pragma once

#include "tawashi_exception.hpp"
#include <boost/container/flat_map.hpp>
#include <string>
#include <istream>
#include <cstddef>

namespace tawashi {
	class UnsupportedContentTypeException : public Exception {
	public:
		explicit UnsupportedContentTypeException (const boost::string_view& parMessage);
	};

	namespace cgi {
		class Env;

		typedef boost::container::flat_map<std::string, std::string> PostMapType;

		const PostMapType& read_post (std::istream& parSrc, const Env& parEnv);
		const PostMapType& read_post (std::istream& parSrc, const Env& parEnv, std::size_t parMaxLen);
	} //namespace cgi
} //namespace tawashi
