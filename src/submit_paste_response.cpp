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

#include "submit_paste_response.hpp"
#include "incredis/incredis.hpp"
#include "cgi_post.hpp"
#include "num_to_token.hpp"
#include <ciso646>

namespace tawashi {
	namespace {
		const char g_post_key[] = "pastie";
	} //unnamed namespace

	SubmitPasteResponse::SubmitPasteResponse (redis::IncRedis& parRedis) :
		Response("text/plain"),
		m_redis(parRedis)
	{
	}

	void SubmitPasteResponse::on_send (std::ostream& parStream) {
		auto post = cgi::read_post(cgi_env());
		auto post_data_it = post.find(g_post_key);
		if (post.end() == post_data_it) {
			parStream << "can't find POST data\n";
		}
		else if (submit_to_redis(post_data_it->second)) {
			parStream << "post submitted correctly\n";
		}
		else {
			parStream << "something happened? :/\n";
		}
	}

	bool SubmitPasteResponse::submit_to_redis (const std::string& parText) const {
		if (not m_redis.is_connected()) {
			m_redis.connect();
			m_redis.wait_for_connect();
			if (not m_redis.is_connected())
				return false;
		}

		const auto next_id = m_redis.incr("paste_counter");
		const std::string token = num_to_token(next_id);
		assert(not token.empty());
		return m_redis.set(token, parText);
	}
} //namespace tawashi
