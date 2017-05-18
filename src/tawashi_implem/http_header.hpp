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
#include <cstdint>
#include <string>
#include <ostream>

namespace tawashi {
	SLOW_ENUM(HttpStatusCodes, uint16_t,
		Code301_MovedPermanently,
		Code302_Found,
		Code303_SeeOther,
		Code400_BadRequest,
		Code403_Forbidden,
		Code404_NotFound,
		Code413_PayloadTooLarge,
		Code429_TooManyRequests,
		Code431_RequestHeaderFieldsTooLarge,
		Code500_InternalServerError,
		Code501_NotImplemented,
		Code503_ServiceUnavailable,
		CodeNone
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
		HttpHeader (Types parType, HttpStatusCodes parCode, std::string&& parParam);
		~HttpHeader() noexcept = default;

		Types type() const { return m_header_type; }
		HttpStatusCodes status_code() const { return m_status_code; }
		const std::string& parameter() const { return m_param; }
		bool body_required() const;

		void set_status (HttpStatusCodes parCode);
		void unset_status();
		void set_type (Types parType, std::string&& parParameter);

	private:
		std::string m_param;
		HttpStatusCodes m_status_code;
		Types m_header_type;
	};

	std::ostream& operator<< (std::ostream& parStream, const HttpHeader& parHeader);
	HttpHeader make_header_type_html();
	HttpHeader make_header_type_text_utf8();
	HttpStatusCodes int_to_status_code (uint16_t parCode);
} //namespace tawashi
