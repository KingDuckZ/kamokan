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
#include <sstream>

namespace tawashi {
	namespace {
		const char g_post_key[] = "pastie";
	} //unnamed namespace

	SubmitPasteResponse::SubmitPasteResponse (const SettingsBag& parSettings) :
		Response(Response::ContentType, "text/plain", "paste", parSettings, true)
	{
	}

	void SubmitPasteResponse::on_process() {
		auto post = cgi::read_post(cgi_env());
		auto post_data_it = post.find(g_post_key);
		if (post.end() == post_data_it) {
			m_error_message = "can't find POST data";
			return;
		}

		boost::optional<std::string> token = submit_to_redis(post_data_it->second);
		if (token) {
			std::ostringstream oss;
			oss << base_uri() << '/' << *token;
			this->change_type(Response::Location, oss.str());
		}
	}

	void SubmitPasteResponse::on_send (std::ostream& parStream) {
		assert(not m_error_message.empty());
		parStream << "something happened? :/\n" <<
			m_error_message << '\n';
	}

	boost::optional<std::string> SubmitPasteResponse::submit_to_redis (const std::string& parText) const {
		auto& redis = this->redis();
		if (not redis.is_connected())
			return boost::optional<std::string>();

		const auto next_id = redis.incr("paste_counter");
		const std::string token = num_to_token(next_id);
		assert(not token.empty());
		if (redis.set(token, parText)) {
			return boost::make_optional(token);
		}
		else {
			return boost::optional<std::string>();
		}
	}
} //namespace tawashi
