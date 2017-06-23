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
#include "lua_scripts_for_redis.hpp"
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

		bool is_valid_token (const boost::string_view& parToken, uint32_t parMaxLen) {
			if (parToken.empty())
				return false;
			if (parMaxLen > 0 and parToken.size() > parMaxLen)
				return false;

			auto it_mark = std::find(parToken.begin(), parToken.end(), '?');
			if (parToken.begin() == it_mark)
				return false;
			for (auto it_ch = parToken.begin(); it_ch != it_mark; ++it_ch) {
				if (*it_ch < 'a' or *it_ch > 'z') {
					spdlog::get("statuslog")->info(
						"Token's byte {} is invalid; value={}",
						it_ch - parToken.begin(),
						static_cast<int>(*it_ch)
					);
					return false;
				}
			}
			return true;
		}
	} //unnamed namespace

	Storage::RetrievedPastie::RetrievedPastie() :
		pastie(),
		lang(),
		self_destructed(false),
		valid_token(false)
	{
	}

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
		using redis::RedisInt;

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
			"selfdes", static_cast<RedisInt>(parSelfDestruct ? 1 : 0))
		) {
			redis.set(parRemoteIP, "");
			redis.expire(parRemoteIP, m_settings->as<uint32_t>("resubmit_wait"));
			if (redis.expire(token, parExpiry))
				return make_submission_result(std::move(token));
		}

		return make_submission_result(ErrorReasons::PastieNotSaved);
	}

	auto Storage::retrieve_pastie (const boost::string_view& parToken, uint32_t parMaxTokenLen) const -> RetrievedPastie {
		RetrievedPastie retval;
		retval.valid_token = is_valid_token(parToken, parMaxTokenLen);
		if (not retval.valid_token)
			return retval;

		redis::Script retrieve = m_redis->command().make_script(boost::string_view(g_load_script, g_load_script_size - 1));
		auto batch = m_redis->command().make_batch();
		retrieve.run(batch, std::make_tuple(parToken), std::make_tuple());
		auto raw_replies = batch.replies();
		if (raw_replies.empty())
			return retval;

		if (raw_replies.front().is_error()) {
			retval.error =
				boost::make_optional<std::string>(get_error_string(raw_replies.front()).message());
			return retval;
		}
		auto pastie_reply = get_array(raw_replies.front());

		retval.pastie = get_string(pastie_reply[0]);
		const redis::RedisInt selfdes = get_integer(pastie_reply[1]);
		const redis::RedisInt deleted = get_integer(pastie_reply[2]);
		retval.lang = get_string(pastie_reply[3]);
		retval.self_destructed = selfdes and deleted;

		if (selfdes and not deleted) {
			auto statuslog = spdlog::get("statuslog");
			statuslog->error("Pastie \"{}\" was marked as self-destructing but DEL failed to delete it", parToken);
		}

#if defined(SPDLOG_DEBUG_ON)
		{
			auto statuslog = spdlog::get("statuslog");
			if (retval.pastie)
				statuslog->debug("Retrieving pastie with token \"{}\" gave a result of size {}", parToken, retval.pastie->size());
			else
				statuslog->debug("Retrieving pastie with token \"{}\" gave no results", parToken);
		}
#endif
		return retval;
	}

#if defined(KAMOKAN_WITH_TESTING)
	const SettingsBag& Storage::settings() const {
		return *m_settings;
	}
#endif
} //namespace kamokan
