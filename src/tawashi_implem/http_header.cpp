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

		inline constexpr uint16_t single_status_code_to_code (const char* parCode) {
			std::size_t idx = 0;
			uint16_t ret_num = 0;
			uint16_t digits = 0;
			char cur_char = 0;
			while ((cur_char = parCode[idx++]) and digits < 3) {
				if (cur_char >= '0' and cur_char <= '9') {
					uint16_t multip = 1;
					for (uint16_t z = 1; z < (3 - digits); ++z) {
						multip *= 10;
					}
					ret_num += multip * (cur_char - '0');
					++digits;
				}
			}
			return ret_num;
		}

		template <int... Values>
		inline constexpr sprout::array<uint16_t, sizeof...(Values)> make_status_codes_lookup (dhandy::bt::number_seq<int, Values...>) {
			return sprout::array<uint16_t, sizeof...(Values)> {
				single_status_code_to_code(HttpStatusCodes::_names()[Values])...
			};
		}

		uint16_t status_code_name_to_num (HttpStatusCodes parCode) {
			constexpr const auto status_codes = make_status_codes_lookup(dhandy::bt::number_range<int, 0, HttpStatusCodes::_size() - 1>());
			return status_codes[parCode._to_integral()];
		}

		constexpr auto g_status_code_descriptions = ::better_enums::make_map(get_status_code_desc);
	} //unnamed namespace

	HttpHeader::HttpHeader() :
		m_param("text/html"),
		m_status_code(HttpStatusCodes::CodeNone),
		m_header_type(ContentType)
	{
	}

	HttpHeader::HttpHeader (Types parType, HttpStatusCodes parCode, std::string&& parParam) :
		m_param(std::move(parParam)),
		m_status_code(parCode),
		m_header_type(parType)
	{
	}

	void HttpHeader::set_status (HttpStatusCodes parCode) {
		m_status_code = parCode;
	}

	void HttpHeader::unset_status() {
		m_status_code = HttpStatusCodes::CodeNone;
	}

	void HttpHeader::set_type (Types parType, std::string&& parParameter) {
		m_header_type = parType;
		m_param = std::move(parParameter);
	}

	std::ostream& operator<< (std::ostream& parStream, const HttpHeader& parHeader) {
		const HttpStatusCodes code_none = HttpStatusCodes::CodeNone;
		if (parHeader.status_code() != code_none) {
			parStream <<
				"Status: " <<
				status_code_name_to_num(parHeader.status_code()) <<
				g_status_code_descriptions[parHeader.status_code()] <<
				'\n'
			;
		}
		switch (parHeader.type()) {
		case HttpHeader::ContentType:
			SPDLOG_TRACE(spdlog::get("statuslog"), "Response is a Content-type (data)");
			parStream << "Content-type: " << parHeader.parameter() << '\n';
			break;
		case HttpHeader::Location:
			SPDLOG_TRACE(spdlog::get("statuslog"), "Response is a Location (redirect)");
			parStream << "Location: " << parHeader.parameter() << '\n';
			break;
		}
		parStream << '\n';
		return parStream;
	}

	HttpHeader make_header_type_html() {
		return HttpHeader(HttpHeader::ContentType, HttpStatusCodes::CodeNone, "text/html");
	}

	HttpHeader make_header_type_text_utf8() {
		return HttpHeader(HttpHeader::ContentType, HttpStatusCodes::CodeNone, "text/plain; charset=utf-8");
	}

	HttpStatusCodes int_to_status_code (uint16_t parCode) {
		constexpr const auto status_codes = make_status_codes_lookup(dhandy::bt::number_range<int, 0, HttpStatusCodes::_size() - 1>());
		auto it_found_code = std::find(status_codes.begin(), status_codes.end(), parCode);
		if (it_found_code != status_codes.end()) {
			const auto index = it_found_code - status_codes.begin();
			assert(index < HttpStatusCodes::_size() - 1);
			return HttpStatusCodes::_from_integral(index);
		}
		else {
			return HttpStatusCodes::CodeNone;
		}
	}

	bool HttpHeader::body_required() const {
		return type() == ContentType;
	}
} //namespace tawashi
