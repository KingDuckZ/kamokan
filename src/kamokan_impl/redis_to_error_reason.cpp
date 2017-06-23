/* Copyright 2017, Michele Santullo
 * This file is part of "kamokan".
 *
 * "kamokan" is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * "kamokan" is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with "kamokan".  If not, see <http://www.gnu.org/licenses/>.
 */

#include "redis_to_error_reason.hpp"
#include "spdlog.hpp"

namespace kamokan {
	tawashi::ErrorReasons redis_to_error_reason (const redis::ErrorString& parError) {
		return redis_to_error_reason(parError.message());
	}

	tawashi::ErrorReasons redis_to_error_reason (const std::string& parError) {
		using tawashi::ErrorReasons;

		try {
			return ErrorReasons::_from_string(parError.c_str());
		}
		catch (const std::runtime_error& e) {
			auto statuslog = spdlog::get("statuslog");
			statuslog->error(
				"Unable to deduce error reason from the received string: \"{}\"; exception raised: \"{}\"",
				parError,
				e.what()
			);
			return ErrorReasons::UnknownReason;
		}
	}
} //namespace kamokan
