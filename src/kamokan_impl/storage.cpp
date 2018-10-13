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
#include "duckhandy/stringize.h"
#include "spdlog.hpp"
#include "truncated_string.hpp"
#include "string_conv.hpp"
#include "lua_scripts_for_redis.hpp"
#include "redis_to_error_reason.hpp"
#include <cassert>
#include <ciso646>
#include <string>
#include <utility>
#include <algorithm>

#define KAMOKAN_TOKEN_PREFIX "kamokan:"
#define TOKEN_PREFIX KAMOKAN_TOKEN_PREFIX "{store:}"

namespace kamokan {
	namespace {
		const char g_hl_token_prefix[] = KAMOKAN_TOKEN_PREFIX "hl:";
		const char g_token_prefix[] = TOKEN_PREFIX;

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

		std::string make_regular_pastie_token (const boost::string_view& parToken) {
			return std::string(g_token_prefix) + std::string(parToken);
		}
	} //unnamed namespace

	Storage::RetrievedPastie::RetrievedPastie() :
		highlighted(false),
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
		using boost::string_view;
		using dhandy::lexical_cast;

		if (not is_connected())
			return make_submission_result(ErrorReasons::RedisDisconnected);

		assert(m_redis);
		auto& redis = *m_redis;

		redis::Script retrieve = m_redis->command().make_script(string_view(g_save_script, g_save_script_size));
		auto batch = m_redis->command().make_batch();
		{
			string_view paste_counter_token(TOKEN_PREFIX "paste_counter");
			std::string prefix(g_token_prefix);
			const auto expiry = lexical_cast<std::string>(parExpiry);
			const auto self_des = string_view(parSelfDestruct ? "1" : "0");
			const auto flood_wait = m_settings->as<string_view>("resubmit_wait");
			retrieve.run(batch,
				std::make_tuple(paste_counter_token, prefix + parRemoteIP),
				std::make_tuple(prefix, parText, expiry, parLang, self_des, flood_wait)
			);
		}
		auto raw_replies = batch.replies();
		auto statuslog = spdlog::get("statuslog");
		if (raw_replies.empty()) {
			statuslog->error("Received empty reply from redis");
			return make_submission_result(ErrorReasons::UnknownReason);
		}

		if (raw_replies.front().is_error()) {
			statuslog->error("Received error reply from redis");
			return make_submission_result(redis_to_error_reason(get_error_string(raw_replies.front())));
		}

		std::string token = get_string(raw_replies.front());

		if (statuslog->should_log(spdlog::level::info)) {
			statuslog->info(
				"Saved pastie of size {} to redis token \"{}\" -> \"{}\"",
				parText.size(),
				token,
				tawashi::truncated_string(parText, 30)
			);
		}
		return make_submission_result(std::move(token));
	}

	auto Storage::retrieve_pastie (
		const boost::string_view& parToken,
		uint32_t parMaxTokenLen,
		const boost::string_view& parRequestedLang
	) const -> RetrievedPastie {

		using boost::string_view;


		RetrievedPastie retval;
		retval.valid_token = is_valid_token(parToken, parMaxTokenLen);
		if (not retval.valid_token)
			return retval;

		redis::Script retrieve = m_redis->command().make_script(string_view(g_load_script, g_load_script_size));
		auto batch = m_redis->command().make_batch();
		std::string token_with_prefix = make_regular_pastie_token(parToken);
		retrieve.run(
			batch,
			std::make_tuple(token_with_prefix),
			std::make_tuple(parRequestedLang)
		);
		auto raw_replies = batch.replies();
		if (raw_replies.empty())
			return retval;

		if (raw_replies.front().is_error()) {
			using std::string;
			using boost::make_optional;
			retval.error =
				make_optional<string>(string(get_error_string(raw_replies.front()).message()));
			return retval;
		}
		auto pastie_reply = get_array(raw_replies.front());

		retval.pastie = get_string(pastie_reply[0]);
		const redis::RedisInt selfdes = get_integer(pastie_reply[1]);
		const redis::RedisInt deleted = get_integer(pastie_reply[2]);
		retval.lang = get_string(pastie_reply[4]);
		retval.self_destructed = selfdes and deleted;
		retval.highlighted = static_cast<bool>(get_integer(pastie_reply[3]));
		retval.comment = get_string(pastie_reply[5]);

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

	void Storage::submit_highlighted_pastie (
		const boost::string_view& parToken,
		const boost::string_view& parText,
		const boost::string_view& parComment,
		uint32_t parMaxTokenLen
	) const {
		using boost::string_view;

		const bool valid_token = is_valid_token(parToken, parMaxTokenLen);
		if (not valid_token)
			return;

		redis::Script retrieve = m_redis->command().make_script(string_view(g_add_highlighted_script, g_add_highlighted_script_size));
		auto batch = m_redis->command().make_batch();
		std::string token_with_prefix(make_regular_pastie_token(parToken));
		retrieve.run(
			batch,
			std::make_tuple(token_with_prefix),
			std::make_tuple(parText, parComment)
		);
	}
} //namespace kamokan

#undef TOKEN_PREFIX
