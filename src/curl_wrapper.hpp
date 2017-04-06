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

#include <boost/utility/string_ref.hpp>
#include <string>
#include <memory>

typedef void CURL;

namespace tawashi {
	class CurlWrapper {
	public:
		typedef std::shared_ptr<CURL> CurlPtr;

		CurlWrapper();
		CurlWrapper (const CurlWrapper&) = delete;
		~CurlWrapper() noexcept;

		std::string url_escape (const boost::string_ref& parText) const;
		std::string url_unescape (const boost::string_ref& parText) const;

	private:
		CurlPtr m_curl;
	};

	std::string unescape_string (const CurlWrapper& parCurl, const boost::string_ref& parString);
} //namespace tawashi
