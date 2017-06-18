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

#include "submit_paste_response.hpp"
#include "storage.hpp"
#include "cgi_post.hpp"
#include "settings_bag.hpp"
#include "duckhandy/compatibility.h"
#include "duckhandy/lexical_cast.hpp"
#include "tawashi_exception.hpp"
#include "ip_utils.hpp"
#include "string_conv.hpp"
#include <ciso646>
#include <algorithm>
#include <boost/lexical_cast.hpp>
#include <cstdint>
#include <spdlog/spdlog.h>
#include <utility>

namespace kamokan {
	namespace {
		const char g_post_key[] = "pastie";
		const char g_language_key[] = "lang";
		const char g_duration_key[] = "ttl";
		const char g_self_destruct[] = "selfdes";

		class MissingPostVarError : public tawashi::Exception {
		public:
			explicit MissingPostVarError(const boost::string_view& parKey) :
				tawashi::Exception(
					tawashi::ErrorReasons::MissingPostVariable,
					"Error retrieving POST variable \"" + std::string(parKey.begin(), parKey.end()) + "\""
				)
			{}
		};

		template <std::size_t N>
		inline boost::string_view make_string_view (const char (&parStr)[N]) a_always_inline;

		template <std::size_t N>
		boost::string_view make_string_view (const char (&parStr)[N]) {
			static_assert(N > 0, "wat?");
			return boost::string_view(parStr, N - 1);
		}

		boost::string_view get_value_from_post (const cgi::PostMapType& parPost, boost::string_view parKey) {
			std::string key(parKey);
			auto post_data_it = parPost.find(key);
			if (parPost.end() == post_data_it)
				throw MissingPostVarError(parKey);
			return post_data_it->second;
		}

		boost::string_view get_value_from_post_log_failure (const cgi::PostMapType& parPost, boost::string_view parKey) {
			try {
				return get_value_from_post(parPost, parKey);
			}
			catch (const MissingPostVarError& e) {
				spdlog::get("statuslog")->info(e.what());
				return boost::string_view();
			}
		}
	} //unnamed namespace

#if defined(KAMOKAN_WITH_TESTING)
	SubmitPasteResponse::SubmitPasteResponse (
		const Kakoune::SafePtr<SettingsBag>& parSettings,
		std::ostream* parStreamOut,
		const Kakoune::SafePtr<cgi::Env>& parCgiEnv,
		bool parInitStorage
	) :
		Response(parSettings, parStreamOut, parCgiEnv, parInitStorage)
	{
	}
#endif

	SubmitPasteResponse::SubmitPasteResponse (
		const Kakoune::SafePtr<SettingsBag>& parSettings,
		std::ostream* parStreamOut,
		const Kakoune::SafePtr<cgi::Env>& parCgiEnv
	) :
		Response(parSettings, parStreamOut, parCgiEnv, true)
	{
	}

	tawashi::HttpHeader SubmitPasteResponse::on_process() {
		using tawashi::ErrorReasons;

		boost::string_view pastie;
		boost::string_view duration;
		bool self_destruct;

		auto statuslog = spdlog::get("statuslog");
		assert(statuslog);

		const SettingsBag& settings = this->settings();
		try {
			auto& post = this->cgi_post();
			pastie = get_value_from_post(post, make_string_view(g_post_key));
			m_pastie_lang = get_value_from_post_log_failure(post, make_string_view(g_language_key));
			duration = get_value_from_post_log_failure(post, make_string_view(g_duration_key));
			self_destruct = string_conv<bool>(get_value_from_post_log_failure(post, make_string_view(g_self_destruct)));
		}
		catch (const tawashi::UnsupportedContentTypeException& err) {
			statuslog->info(
				"Unsupported content type exception: \"{}\"",
				err.what()
			);
			return make_error_redirect(ErrorReasons::UnsupportedContentType);
		}
		catch (const tawashi::Exception& e) {
			statuslog->error(e.what());
			return make_error_redirect(e.reason());
		}

		const auto max_sz = settings.as<uint32_t>("max_pastie_size");
		if (pastie.size() < settings.as<uint32_t>("min_pastie_size")) {
			return make_error_redirect(ErrorReasons::PostLengthNotInRange);
		}
		if (max_sz and pastie.size() > max_sz) {
			if (settings.as<bool>("truncate_long_pasties")) {
				pastie = pastie.substr(0, max_sz);
			}
			else {
				return make_error_redirect(ErrorReasons::PostLengthNotInRange);
			}
		}

		//TODO: replace boost's lexical_cast with mine when I have some checks
		//over invalid inputs
		const uint32_t duration_int = std::max(std::min((duration.empty() ? 86400U : boost::lexical_cast<uint32_t>(duration)), 2628000U), 1U);
		StringOrHeader submit_result = submit_to_storage(pastie, duration_int, m_pastie_lang, self_destruct);
		const boost::optional<std::string>& token = submit_result.first;

		if (token) {
			m_pastie_token = std::move(*token);
			std::ostringstream oss;
			oss << m_pastie_token;
			if (not m_pastie_lang.empty())
				oss << '?' << m_pastie_lang;

			std::string redirect = oss.str();
			statuslog->info("Pastie token=\"{}\" redirect=\"{}\"", m_pastie_token, redirect);

			if (self_destruct)
				return tawashi::make_header_type_html();
			else
				return this->make_success_response(std::move(redirect));
		}
		else {
			statuslog->info("Empty pastie token (possibly due to a previous failure)");
			return submit_result.second;
		}
	}

	auto SubmitPasteResponse::submit_to_storage (
		const boost::string_view& parText,
		uint32_t parExpiry,
		const boost::string_view& parLang,
		bool parSelfDestruct
	) -> StringOrHeader {
		auto& storage = this->storage();
		std::string remote_ip = tawashi::guess_real_remote_ip(cgi_env());
		Storage::SubmissionResult submission_res = storage.submit_pastie(parText, parExpiry, parLang, parSelfDestruct, remote_ip);
		if (not submission_res.error)
			return std::make_pair(boost::make_optional(std::move(submission_res.token)), tawashi::HttpHeader());
		else
			return std::make_pair(boost::optional<std::string>(), make_error_redirect(*submission_res.error));
	}

	tawashi::HttpHeader SubmitPasteResponse::make_success_response (std::string&& parPastieParam) {
		using tawashi::HttpStatusCodes;
		return this->make_redirect(HttpStatusCodes::Code303_SeeOther, std::move(parPastieParam));
	}

	void SubmitPasteResponse::on_mustache_prepare (mstch::map& parContext) {
		parContext["pastie_token"] = std::move(m_pastie_token);
		parContext["pastie_lang"] = std::move(m_pastie_lang);
	}
} //namespace kamokan
