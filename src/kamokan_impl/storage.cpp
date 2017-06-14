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

#include "storage.hpp"
#include "settings_bag.hpp"
#include "incredis/incredis.hpp"
#include "num_to_token.hpp"
#include "duckhandy/stringize.h"
#include "spdlog.hpp"
#include "truncated_string.hpp"
#include "string_conv.hpp"
#include <cassert>
#include <ciso646>
#include <string>
#include <utility>
#include <algorithm>

namespace kamokan {
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
			using tawashi::ErrorReasons;
			return Storage::SubmissionResult { std::move(parToken), boost::optional<ErrorReasons>() };
		}

		Storage::SubmissionResult make_submission_result (tawashi::ErrorReasons parError) {
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
		SPDLOG_TRACE(spdlog::get("statuslog"), "Trying to connect to Redis asynchronously");
	}

	bool Storage::is_connected() const {
		return m_redis and m_redis->is_connected();
	}

	void Storage::finalize_connection() {
		SPDLOG_TRACE(spdlog::get("statuslog"), "Asked Storage to finalize the Redis connection");
		if (m_redis) {
			SPDLOG_TRACE(spdlog::get("statuslog"), "Finalizing Redis connection");
			m_redis->wait_for_connect();
			auto batch = m_redis->make_batch();
			batch.select(m_settings->as<uint32_t>("redis_db"));
			batch.client_setname("kamokan_v" STRINGIZE(VERSION_MAJOR) "." STRINGIZE(VERSION_MINOR) "." STRINGIZE(VERSION_PATCH));
			batch.throw_if_failed();
		}
	}

	Storage::SubmissionResult Storage::submit_pastie (
		const boost::string_view& parText,
		uint32_t parExpiry,
		const boost::string_view& parLang,
		bool parSelfDestruct,
		const std::string& parRemoteIP
	) const {
		using tawashi::ErrorReasons;

		if (not is_connected())
			return make_submission_result(ErrorReasons::RedisDisconnected);

		assert(m_redis);
		auto& redis = *m_redis;
		if (redis.get(parRemoteIP)) {
			//please wait and submit again
			return make_submission_result(ErrorReasons::UserFlooding);
		}

		auto statuslog = spdlog::get("statuslog");
		if (statuslog->should_log(spdlog::level::info)) {
			statuslog->info(
				"Submitting pastie of size {} to redis -> \"{}\"",
				parText.size(),
				tawashi::truncated_string(parText, 30)
			);
		}

		const auto next_id = redis.incr("paste_counter");
		std::string token = num_to_token(next_id);
		assert(not token.empty());
		if (redis.hmset(token,
			"pastie", parText,
			"max_ttl", dhandy::lexical_cast<std::string>(parExpiry),
			"lang", parLang,
			"selfdes", (parSelfDestruct ? "1" : "0"))
		) {
			redis.set(parRemoteIP, "");
			redis.expire(parRemoteIP, m_settings->as<uint32_t>("resubmit_wait"));
			if (redis.expire(token, parExpiry))
				return make_submission_result(std::move(token));
		}

		return make_submission_result(ErrorReasons::PastieNotSaved);
	}

	boost::optional<std::string> Storage::retrieve_pastie (const boost::string_view& parToken) const {
		using opt_string = redis::IncRedis::opt_string;
		using opt_string_list = redis::IncRedis::opt_string_list;

		opt_string_list pastie_reply = m_redis->hmget(parToken, "pastie", "selfdes");
		opt_string pastie = (pastie_reply and not pastie_reply->empty() ? (*pastie_reply)[0] : opt_string());
		opt_string selfdes = (pastie_reply and not pastie_reply->size() > 1 ? (*pastie_reply)[1] : opt_string());

		if (selfdes and string_conv<bool>(*selfdes)) {
			const bool deleted = m_redis->del(parToken);
			if (not deleted) {
				auto statuslog = spdlog::get("statuslog");
				statuslog->error("Pastie \"{}\" was marked as self-destructing but DEL failed to delete it", parToken);
			}
		}

#if defined(SPDLOG_DEBUG_ON)
		{
			auto statuslog = spdlog::get("statuslog");
			if (pastie)
				statuslog->debug("Retrieving pastie with token \"{}\" gave a result of size {}", parToken, pastie->size());
			else
				statuslog->debug("Retrieving pastie with token \"{}\" gave no results", parToken);
		}
#endif
		return pastie;
	}

#if defined(KAMOKAN_WITH_TESTING)
	const SettingsBag& Storage::settings() const {
		return *m_settings;
	}
#endif
} //namespace kamokan