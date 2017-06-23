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

#include "general_pastie_response.hpp"
#include "cgi_env.hpp"
#include "settings_bag.hpp"
#include "error_reasons.hpp"
#include "redis_to_error_reason.hpp"
#include <cassert>
#include <ciso646>
#include <utility>

namespace kamokan {
	namespace {
		boost::string_view get_search_token (const cgi::Env& parCgiEnv) {
			return cgi::drop_arguments(parCgiEnv.request_uri_relative());
		}
	} //unnamed namespace

	GeneralPastieResponse::GeneralPastieResponse (
		const Kakoune::SafePtr<SettingsBag>& parSettings,
		std::ostream* parStreamOut,
		const Kakoune::SafePtr<cgi::Env>& parCgiEnv
	) :
		Response(parSettings, parStreamOut, parCgiEnv, true)
	{
	}

	tawashi::HttpHeader GeneralPastieResponse::on_process() {
		using tawashi::ErrorReasons;

		boost::string_view token = get_search_token(cgi_env());
		m_pastie_info =
			storage().retrieve_pastie(token, settings().as<uint32_t>("max_token_length"));

		if (m_pastie_info.error)
			return make_error_redirect(redis_to_error_reason(*m_pastie_info.error));

		if (this->token_invalid()) {
			assert(this->pastie_not_found());
			return make_error_redirect(ErrorReasons::InvalidToken);
		}
		if (this->pastie_not_found())
			return make_error_redirect(ErrorReasons::PastieNotFound);

		assert(m_pastie_info.pastie);
		assert(not token_invalid());
		assert(not pastie_not_found());

		return this->on_general_pastie_process();
	}

	void GeneralPastieResponse::on_mustache_prepare (mstch::map& parContext) {
		assert(m_pastie_info.pastie);
		assert(not token_invalid());
		assert(not pastie_not_found());

		auto pastie_lang = (m_pastie_info.lang ? boost::string_view(*m_pastie_info.lang) : boost::string_view());
		parContext["self_destructed"] = m_pastie_info.self_destructed;
		parContext["pastie_token"] = get_search_token(cgi_env());
		parContext["pastie_lang"] = pastie_lang;
		parContext["colourless"] = pastie_lang.empty() or pastie_lang == "colourless";

		this->on_general_mustache_prepare(std::move(*m_pastie_info.pastie), parContext);
	}

	bool GeneralPastieResponse::token_invalid() const {
		assert(m_pastie_info.valid_token or not m_pastie_info.pastie);
		return not m_pastie_info.valid_token;
	}

	bool GeneralPastieResponse::pastie_not_found() const {
		return not m_pastie_info.pastie;
	}

	std::string GeneralPastieResponse::default_pastie_lang() {
		if (m_pastie_info.lang)
			return *m_pastie_info.lang;
		else
			return std::string();
	}
} //namespace kamokan
