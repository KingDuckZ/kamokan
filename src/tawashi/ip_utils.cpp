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

#include "ip_utils.hpp"
#include "cgi_env.hpp"
#include "tawashi_config.h"
#include "duckhandy/int_conv.hpp"
#include <boost/utility/string_view.hpp>
#include <spdlog/spdlog.h>
#include <cassert>
#include <algorithm>
#include <ciso646>

#if !defined(TAWASHI_WITH_IP_LOGGING)
extern "C" void tiger (const char* parStr, uint64_t parLength, uint64_t parHash[3], char parPadding);
#endif

namespace tawashi {
	namespace {
		std::string hash_if_configured (const std::string& parIP) a_always_inline;

#if !defined(TAWASHI_WITH_IP_LOGGING)
		std::string hashed_ip (const std::string& parIP) {
			uint64_t hash[3];
			tiger(parIP.data(), parIP.size(), hash, 0x80);

			auto h1_ary = dhandy::int_to_ary<char, 16>(hash[0]);
			auto h2_ary = dhandy::int_to_ary<char, 16>(hash[1]);
			auto h3_ary = dhandy::int_to_ary<char, 16>(hash[2]);
			auto h1 = h1_ary.to<boost::string_view>();
			auto h2 = h2_ary.to<boost::string_view>();
			auto h3 = h3_ary.to<boost::string_view>();

			std::string retval(2 * sizeof(uint64_t) * 3, '0');
			assert(h1.size() <= 2 * sizeof(uint64_t));
			std::copy(h1.begin(), h1.end(), retval.begin() + 2 * sizeof(uint64_t) * 0 + 2 * sizeof(uint64_t) - h1.size());
			assert(h2.size() <= 2 * sizeof(uint64_t));
			std::copy(h2.begin(), h2.end(), retval.begin() + 2 * sizeof(uint64_t) * 1 +  2 * sizeof(uint64_t) - h2.size());
			assert(h3.size() <= 2 * sizeof(uint64_t));
			std::copy(h3.begin(), h3.end(), retval.begin() + 2 * sizeof(uint64_t) * 2 +  2 * sizeof(uint64_t) - h3.size());

			SPDLOG_DEBUG(spdlog::get("statuslog"), "IP \"{}\" hashed -> \"{}\"", parIP, retval);
			assert(retval.size() == 16 * 3);
			return retval;
		}
#endif

		inline std::string hash_if_configured (const std::string& parIP) {
#if defined(TAWASHI_WITH_IP_LOGGING)
			return parIP;
#else
			return hashed_ip(parIP);
#endif
		}

	} //unnamed namespace

	//see: https://stackoverflow.com/questions/18799808/how-do-i-count-unique-visitors-to-my-site
	std::string guess_real_remote_ip (const cgi::Env& parCgiEnv) {
		if (not parCgiEnv.http_client_ip().empty()) {
			return hash_if_configured(parCgiEnv.http_client_ip());
		}
		else if (not parCgiEnv.http_x_forwarded_for().empty()) {
			return hash_if_configured(parCgiEnv.http_x_forwarded_for());
		}
		else {
			return hash_if_configured(parCgiEnv.remote_addr());
		}
	}
} //namespace tawashi
