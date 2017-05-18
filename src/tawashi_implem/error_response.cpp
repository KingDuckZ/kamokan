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

#include "error_response.hpp"
#include "error_reasons.hpp"
#include "cgi_env.hpp"
#include "sprout/array/array.hpp"
#include "string_lengths.hpp"
#include <boost/algorithm/string/replace.hpp>
#include <boost/lexical_cast.hpp>
#include <ciso646>
#include <string>
#include <cassert>

namespace tawashi {
	ErrorResponse::ErrorResponse (
		const Kakoune::SafePtr<SettingsBag>& parSettings,
		std::ostream* parStreamOut,
		const Kakoune::SafePtr<cgi::Env>& parCgiEnv
	) :
		Response(parSettings, parStreamOut, parCgiEnv, false)
	{
	}

	void ErrorResponse::on_mustache_prepare (mstch::map& parContext) {
		auto get = cgi_env().query_string_split();
		const int reason_int = boost::lexical_cast<int>(get["reason"]);
		ErrorReasons reason_code(ErrorReasons::UnkownReason);
		if (reason_int >= 0 and reason_int < ErrorReasons::_size())
			reason_code = ErrorReasons::_from_integral(reason_int);

		constexpr const sprout::array<const char*, ErrorReasons::_size()> err_descs {
			"Submitted pastie is either too short or too long and was rejected.",
			"Submitted pastie couldn't be saved.",
			"The pastie was not saved because the client is submitting too many pasties too quickly. Please wait a bit longer and try again.",
			"An unknown error was raised.",
			"Unable to connect to Redis.",
			"Request is missing a POST variable."
		};
		constexpr const auto lengths = string_lengths(err_descs);
		static_assert(err_descs.static_size == lengths.static_size, "Mismatching array sizes between strings and their lengths");

#if !defined(NDEBUG)
		for (std::size_t z = 0; z < err_descs.size(); ++z) {
			assert(std::strlen(err_descs[z]) == lengths[z]);
		}
#endif

		parContext["error_message"] = std::string(err_descs[reason_code], lengths[reason_code]);
		parContext["error_id"] = std::to_string(reason_code);
	}
} //namespace tawashi
