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

#include "pastie_retrieving_response.hpp"
#include "cgi_env.hpp"
#include "settings_bag.hpp"
#include "error_reasons.hpp"
#include <cassert>
#include <ciso646>
#include <boost/algorithm/string/find_iterator.hpp>
#include <boost/range/iterator_range_core.hpp>
#include <boost/algorithm/string/finder.hpp>
#include <boost/range/adaptor/transformed.hpp>
#include <utility>

namespace kamokan {
	namespace {
		mstch::array pastie_to_numbered_lines (boost::string_view parPastie) {
			using boost::string_view;
			using string_view_iterator = string_view::const_iterator;
			using boost::split_iterator;
			using boost::token_finder;
			using boost::adaptors::transformed;
			using MatchRange = boost::iterator_range<string_view_iterator>;

			int line_num = 1;
			return boost::copy_range<mstch::array>(
				boost::make_iterator_range(
					split_iterator<string_view_iterator>(parPastie, token_finder([](char c){return '\n'==c;})),
					split_iterator<string_view_iterator>()
				) |
				transformed([&line_num,parPastie](const MatchRange& r) {
					return mstch::map {
						{"number", line_num++},
						{"line", parPastie.substr(r.begin() - parPastie.begin(), r.size())}
					};
				})
			);
		}

		boost::string_view get_search_token (const cgi::Env& parCgiEnv) {
			return cgi::drop_arguments(parCgiEnv.request_uri_relative());
		}
	} //unnamed namespace

	PastieRetrievingResponse::PastieRetrievingResponse (
		const Kakoune::SafePtr<SettingsBag>& parSettings,
		std::ostream* parStreamOut,
		const Kakoune::SafePtr<cgi::Env>& parCgiEnv
	) :
		Response(parSettings, parStreamOut, parCgiEnv, true)
	{
	}

	tawashi::HttpHeader PastieRetrievingResponse::on_process() {
		using tawashi::ErrorReasons;

		boost::string_view token = get_search_token(cgi_env());
		m_pastie_info =
			storage().retrieve_pastie(token, settings().as<uint32_t>("max_token_length"));

		if (this->token_invalid()) {
			assert(this->pastie_not_found());
			return make_error_redirect(ErrorReasons::InvalidToken);
		}
		if (this->pastie_not_found())
			return make_error_redirect(ErrorReasons::PastieNotFound);

		assert(m_pastie_info.pastie);
		assert(not token_invalid());
		assert(not pastie_not_found());

		return this->on_retrieving_process();
	}

	void PastieRetrievingResponse::on_mustache_prepare (mstch::map& parContext) {
		assert(m_pastie_info.pastie);
		assert(not token_invalid());
		assert(not pastie_not_found());

		parContext["pastie"] =
			this->on_pastie_prepare(std::move(*m_pastie_info.pastie));
		parContext["pastie_lines"] = pastie_to_numbered_lines(
			boost::get<std::string>(parContext["pastie"])
		);
		parContext["self_destructed"] = m_pastie_info.self_destructed;
		parContext["pastie_token"] = get_search_token(cgi_env());
		parContext["pastie_page"] = true;
	}

	bool PastieRetrievingResponse::token_invalid() const {
		assert(m_pastie_info.valid_token or not m_pastie_info.pastie);
		return not m_pastie_info.valid_token;
	}

	bool PastieRetrievingResponse::pastie_not_found() const {
		return not m_pastie_info.pastie;
	}
} //namespace kamokan
