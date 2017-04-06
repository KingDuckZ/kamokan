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

#include "pastie_response.hpp"
#include "incredis/incredis.hpp"
#include <ciso646>

namespace tawashi {
	PastieResponse::PastieResponse (redis::IncRedis& parRedis) :
		Response(Response::ContentType, "text/plain"),
		m_redis(parRedis)
	{
	}

	void PastieResponse::on_send (std::ostream& parStream) {
		using opt_string = redis::IncRedis::opt_string;

		if (cgi_env().path_info().empty()) {
			return;
		}

		auto token = boost::string_ref(cgi_env().path_info()).substr(1);
		opt_string pastie = m_redis.get(token);
		if (not pastie) {
		}

		parStream << *pastie;
	}
} //namespace tawashi
