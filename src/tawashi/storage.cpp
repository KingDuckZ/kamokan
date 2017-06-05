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

#include "storage.hpp"
#include "settings_bag.hpp"
#include "incredis/incredis.hpp"
#include "num_to_token.hpp"
#include "tawashi_config.h"
#include "duckhandy/stringize.h"
#include <cassert>
#include <ciso646>
#include <string>
#include <spdlog/spdlog.h>
#include <utility>

namespace tawashi {
	namespace {
		redis::IncRedis make_incredis (const SettingsBag& parSettings) {
			using redis::IncRedis;

			if (parSettings["redis_mode"] == "inet") {
				return IncRedis(
					parSettings.as<std::string>("redis_server"),
					parSettings.as<uint16_t>("redis_port")
				);
			}
			else if (parSettings["redis_mode"] == "sock") {
				return IncRedis(parSettings.as<std::string>("redis_sock"));
			}
			else {
				throw std::runtime_error("Unknown setting for \"redis_mode\", valid settings are \"inet\" or \"sock\"");
			}
		}

		Storage::SubmissionResult make_submission_result (std::string&& parToken) {
			return Storage::SubmissionResult { std::move(parToken), boost::optional<ErrorReasons>() };
		}

		Storage::SubmissionResult make_submission_result (ErrorReasons parError) {
			return Storage::SubmissionResult { std::string(), boost::make_optional(parError) };
		}
	} //unnamed namespace

	Storage::Storage (const Kakoune::SafePtr<SettingsBag>& parSettings) :
		m_redis(nullptr),
		m_settings(parSettings)
	{
	}

	Storage::~Storage() = default;

	void Storage::connect_async() {
		using redis::IncRedis;

		assert(not m_redis);
		m_redis = std::make_unique<redis::IncRedis>(make_incredis(*m_settings));
		m_redis->connect();
	}

	bool Storage::is_connected() const {
		return m_redis and m_redis->is_connected();
	}

	void Storage::finalize_connection() {
		if (m_redis) {
			SPDLOG_TRACE(spdlog::get("statuslog"), "Finalizing redis connection");
			m_redis->wait_for_connect();
			auto batch = m_redis->make_batch();
			batch.select(m_settings->as<uint32_t>("redis_db"));
			batch.client_setname("tawashi_v" STRINGIZE(VERSION_MAJOR) "." STRINGIZE(VERSION_MINOR) "." STRINGIZE(VERSION_PATCH));
			batch.throw_if_failed();
		}
	}

	Storage::SubmissionResult Storage::submit_pastie (
		const boost::string_ref& parText,
		uint32_t parExpiry,
		const boost::string_ref& parLang,
		const std::string& parRemoteIP
	) const {
		if (not is_connected())
			return make_submission_result(ErrorReasons::RedisDisconnected);

		assert(m_redis);
		auto& redis = *m_redis;
		if (redis.get(parRemoteIP)) {
			//please wait and submit again
			return make_submission_result(ErrorReasons::UserFlooding);
		}

		const auto next_id = redis.incr("paste_counter");
		std::string token = num_to_token(next_id);
		assert(not token.empty());
		if (redis.hmset(token,
			"pastie", parText,
			"max_ttl", dhandy::lexical_cast<std::string>(parExpiry),
			"lang", parLang)
		) {
			redis.set(parRemoteIP, "");
			redis.expire(parRemoteIP, m_settings->as<uint32_t>("resubmit_wait"));
			if (redis.expire(token, parExpiry))
				return make_submission_result(std::move(token));
		}

		return make_submission_result(ErrorReasons::PastieNotSaved);
	}

	boost::optional<std::string> Storage::retrieve_pastie (const boost::string_ref& parToken) const {
		using opt_string = redis::IncRedis::opt_string;
		using opt_string_list = redis::IncRedis::opt_string_list;

		opt_string_list pastie_reply = m_redis->hmget(parToken, "pastie");
		opt_string pastie = (pastie_reply and not pastie_reply->empty() ? (*pastie_reply)[0] : opt_string());

		return pastie;
	}

#if defined(TAWASHI_WITH_TESTING)
	const SettingsBag& Storage::settings() const {
		return *m_settings;
	}
#endif
} //namespace tawashi
