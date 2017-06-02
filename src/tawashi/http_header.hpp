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

#include "enum.h"
#include "mime_split.hpp"
#include <cstdint>
#include <string>
#include <ostream>

namespace tawashi {
	BETTER_ENUM(HttpStatusCodes, uint16_t,
		Code301_MovedPermanently = 301,
		Code302_Found = 302,
		Code303_SeeOther = 303,
		Code400_BadRequest = 400,
		Code403_Forbidden = 403,
		Code404_NotFound = 404,
		Code413_PayloadTooLarge = 413,
		Code429_TooManyRequests = 429,
		Code431_RequestHeaderFieldsTooLarge = 431,
		Code500_InternalServerError = 500,
		Code501_NotImplemented = 501,
		Code503_ServiceUnavailable = 503,
		CodeNone = 0
	)
	class HttpHeader {
	public:
		enum Types : uint8_t {
			ContentType,
			Location,
			Status
		};

		HttpHeader();
		HttpHeader (const HttpHeader&) = default;
		HttpHeader (HttpHeader&&) = default;
		HttpHeader (Types parType, HttpStatusCodes parCode, SplitMime&& parMime);
		HttpHeader (HttpStatusCodes parCode, std::string&& parRedirectLocation);
		~HttpHeader() noexcept = default;

		Types type() const { return m_header_type; }
		HttpStatusCodes status_code() const { return m_status_code; }
		const std::string& redirect_location() const { return m_redirect_location; }
		const SplitMime& mime() const { return m_mime; }
		bool body_required() const;

		void set_status (HttpStatusCodes parCode);
		void unset_status();
		void set_type (Types parType, SplitMime&& parParameter);

	private:
		SplitMime m_mime;
		std::string m_redirect_location;
		HttpStatusCodes m_status_code;
		Types m_header_type;
	};

	std::ostream& operator<< (std::ostream& parStream, const HttpHeader& parHeader);
	HttpHeader make_header_type_html();
	HttpHeader make_header_type_text_utf8();
	[[gnu::pure]] std::string mime_to_string (const HttpHeader& parHeader);
} //namespace tawashi
