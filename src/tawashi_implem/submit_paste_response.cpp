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
#include "cgi_env.hpp"
#include "num_to_token.hpp"
#include "settings_bag.hpp"
#include "duckhandy/compatibility.h"
#include "duckhandy/lexical_cast.hpp"
#include "duckhandy/int_to_string_ary.hpp"
#include <ciso646>
#include <sstream>
#include <stdexcept>
#include <algorithm>
#include <boost/lexical_cast.hpp>
#include <cstdint>
#include <spdlog/spdlog.h>

extern "C" void tiger (const char* parStr, uint64_t parLength, uint64_t parHash[3], char parPadding);

namespace tawashi {
	namespace {
		const char g_post_key[] = "pastie";
		const char g_language_key[] = "lang";
		const char g_duration_key[] = "ttl";

		class MissingPostVarError : public std::runtime_error {
		public:
			MissingPostVarError (const std::string& parMsg) : std::runtime_error(parMsg) {}
		};

		template <std::size_t N>
		inline boost::string_ref make_string_ref (const char (&parStr)[N]) a_always_inline;

		template <std::size_t N>
		boost::string_ref make_string_ref (const char (&parStr)[N]) {
			static_assert(N > 0, "wat?");
			return boost::string_ref(parStr, N - 1);
		}

		boost::string_ref get_value_from_post (const cgi::PostMapType& parPost, boost::string_ref parKey) {
			std::string key(parKey.data(), parKey.size());
			auto post_data_it = parPost.find(key);
			if (parPost.end() == post_data_it) {
				std::ostringstream oss;
				oss << "can't find POST data field \"" << parKey << '"';
				throw MissingPostVarError(oss.str());
			}
			return post_data_it->second;
		}

		std::string hashed_ip (const std::string& parIP) {
			using dhandy::tags::hex;

			uint64_t hash[3];
			tiger(parIP.data(), parIP.size(), hash, 0x80);

			auto h1 = dhandy::int_to_string_ary<char, hex>(hash[0]);
			auto h2 = dhandy::int_to_string_ary<char, hex>(hash[1]);
			auto h3 = dhandy::int_to_string_ary<char, hex>(hash[2]);

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
	} //unnamed namespace

	SubmitPasteResponse::SubmitPasteResponse (
		const Kakoune::SafePtr<ResponseFactory>& parFactory,
		const Kakoune::SafePtr<SettingsBag>& parSettings,
		std::ostream* parStreamOut,
		const Kakoune::SafePtr<cgi::Env>& parCgiEnv
	) :
		Response(parFactory, parSettings, parStreamOut, parCgiEnv, true)
	{
		this->change_type(Response::ContentType, "text/plain");
	}

	void SubmitPasteResponse::on_process() {
		auto post = cgi::read_post(std::cin, cgi_env());
		boost::string_ref pastie;
		boost::string_ref lang;
		boost::string_ref duration;
		try {
			pastie = get_value_from_post(post, make_string_ref(g_post_key));
		}
		catch (const MissingPostVarError& e) {
			m_error_message = e.what();
			return;
		}
		try {
			lang = get_value_from_post(post, make_string_ref(g_language_key));
			duration = get_value_from_post(post, make_string_ref(g_duration_key));
		}
		catch (const MissingPostVarError&) {
		}

		const SettingsBag& settings = this->settings();
		const auto max_sz = settings.as<uint32_t>("max_pastie_size");
		if (pastie.size() < settings.as<uint32_t>("min_pastie_size"))
			return;
		if (max_sz and pastie.size() > max_sz) {
			if (settings.as<bool>("truncate_long_pasties"))
				pastie = pastie.substr(0, max_sz);
			else
				return;
		}

		//TODO: replace boost's lexical_cast with mine when I have some checks
		//over invalid inputs
		const uint32_t duration_int = std::max(std::min((duration.empty() ? 86400U : boost::lexical_cast<uint32_t>(duration)), 2628000U), 1U);
		boost::optional<std::string> token = submit_to_redis(pastie, duration_int, lang);
		if (token) {
			std::ostringstream oss;
			oss << base_uri() << '/' << *token;
			if (not lang.empty())
				oss << '?' << lang;
			this->change_type(Response::Location, oss.str());
		}
	}

	boost::optional<std::string> SubmitPasteResponse::submit_to_redis (const boost::string_ref& parText, uint32_t parExpiry, const boost::string_ref& parLang) const {
		auto& redis = this->redis();
		if (not redis.is_connected())
			return boost::optional<std::string>();

		std::string ip_hash = hashed_ip(cgi_env().remote_addr());
		if (redis.get(ip_hash)) {
			//please wait and submit again
			return boost::optional<std::string>();
		}

		const auto next_id = redis.incr("paste_counter");
		const std::string token = num_to_token(next_id);
		assert(not token.empty());
		if (redis.hmset(token,
			"pastie", parText,
			"max_ttl", dhandy::lexical_cast<std::string>(parExpiry),
			"lang", parLang)
		) {
			redis.set(ip_hash, "");
			redis.expire(ip_hash, settings().as<uint32_t>("resubmit_wait"));
			if (redis.expire(token, parExpiry))
				return boost::make_optional(token);
		}

		return boost::optional<std::string>();
	}
} //namespace tawashi
