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

#include "http_header.hpp"
#include "duckhandy/lexical_cast.hpp"
#include "duckhandy/sequence_bt.hpp"
#include "sprout/array/array.hpp"
#include <utility>
#include <spdlog/spdlog.h>
#include <ciso646>

namespace tawashi {
	namespace {
		constexpr const char* get_status_code_desc (HttpStatusCodes parCode) {
			switch (parCode) {
			case HttpStatusCodes::Code301_MovedPermanently: return "Moved Permanently";
			case HttpStatusCodes::Code302_Found: return "Found";
			case HttpStatusCodes::Code303_SeeOther: return "See Other";
			case HttpStatusCodes::Code400_BadRequest: return "Bad Request";
			case HttpStatusCodes::Code403_Forbidden: return "Forbidden";
			case HttpStatusCodes::Code404_NotFound: return "Not Found";
			case HttpStatusCodes::Code413_PayloadTooLarge: return "Payload Too Large";
			case HttpStatusCodes::Code429_TooManyRequests: return "Too Many Requests";
			case HttpStatusCodes::Code431_RequestHeaderFieldsTooLarge: return "Request Header Fields Too Large";
			case HttpStatusCodes::Code500_InternalServerError: return "Internal Server Error";
			case HttpStatusCodes::Code501_NotImplemented: return "Not Implemented";
			case HttpStatusCodes::Code503_ServiceUnavailable: return "Service Unavailable";
			}
			return "INVALID STATUS CODE";
		}

		constexpr auto g_status_code_descriptions = ::better_enums::make_map(get_status_code_desc);
	} //unnamed namespace

	HttpHeader::HttpHeader() :
		m_mime {"text", "html", {}},
		m_status_code(HttpStatusCodes::CodeNone),
		m_header_type(ContentType)
	{
	}

	HttpHeader::HttpHeader (Types parType, HttpStatusCodes parCode, SplitMime&& parMime) :
		m_mime(std::move(parMime)),
		m_status_code(parCode),
		m_header_type(parType)
	{
	}

	HttpHeader::HttpHeader (HttpStatusCodes parCode, std::string&& parRedirectLocation) :
		m_redirect_location(std::move(parRedirectLocation)),
		m_status_code(parCode),
		m_header_type(Location)
	{
	}

	void HttpHeader::set_status (HttpStatusCodes parCode) {
		m_status_code = parCode;
	}

	void HttpHeader::unset_status() {
		m_status_code = HttpStatusCodes::CodeNone;
	}

	void HttpHeader::set_type (Types parType, SplitMime&& parParameter) {
		m_header_type = parType;
		m_mime = std::move(parParameter);
	}

	bool HttpHeader::body_required() const {
		return type() == ContentType;
	}

	std::ostream& operator<< (std::ostream& parStream, const HttpHeader& parHeader) {
		const HttpStatusCodes code_none = HttpStatusCodes::CodeNone;
		if (parHeader.status_code() != code_none) {
			parStream <<
				"Status: HTTP/1.1 " <<
				parHeader.status_code()._to_integral() <<
				' ' <<
				g_status_code_descriptions[parHeader.status_code()] <<
				'\n'
			;
		}
		switch (parHeader.type()) {
		case HttpHeader::ContentType:
			SPDLOG_TRACE(spdlog::get("statuslog"), "Response is a Content-type (data)");
			parStream << "Content-type: " << mime_to_string(parHeader) << '\n';
			break;
		case HttpHeader::Location:
			SPDLOG_TRACE(spdlog::get("statuslog"), "Response is a Location (redirect)");
			parStream << "Location: " << parHeader.redirect_location() << '\n';
			break;
		}
		parStream << '\n';
		return parStream;
	}

	HttpHeader make_header_type_html() {
		return HttpHeader(
			HttpHeader::ContentType,
			HttpStatusCodes::CodeNone,
			SplitMime { "text", "html", {}}
		);
	}

	HttpHeader make_header_type_text_utf8() {
		return HttpHeader(
			HttpHeader::ContentType,
			HttpStatusCodes::CodeNone,
			SplitMime {"text", "plain", MimeParametersMapType {{"charset", "utf-8"}}}
		);
	}

	std::string mime_to_string (const HttpHeader& parHeader) {
		bool write_ok;
		std::string retval = mime_to_string(parHeader.mime(), write_ok);
		if (not write_ok) {
			assert(false);
			return std::string();
		}
		return retval;
	}
} //namespace tawashi
