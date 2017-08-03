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

#include "pastie_response.hpp"
#include "storage.hpp"
#include "settings_bag.hpp"
#include "escapist.hpp"
#include "cgi_env.hpp"
#include "spdlog.hpp"
#include "highlight_functions.hpp"
#include <ciso646>
#include <algorithm>
#include <cassert>
#include <boost/algorithm/string/find_iterator.hpp>
#include <boost/range/iterator_range_core.hpp>
#include <boost/algorithm/string/finder.hpp>
#include <boost/range/adaptor/transformed.hpp>

namespace kamokan {
	namespace {
		const char g_nolang_token[] = "colourless";

		mstch::array pastie_to_numbered_lines (boost::string_view parPastie) {
			using boost::string_view;
			using string_view_iterator = string_view::const_iterator;
			using boost::split_iterator;
			using boost::token_finder;
			using boost::adaptors::transformed;
			using MatchRange = boost::iterator_range<string_view_iterator>;

			if (not parPastie.empty() and parPastie[parPastie.size() - 1] == '\n')
				parPastie = parPastie.substr(0, parPastie.size() - 1);

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
	} //unnamed namespace

	PastieResponse::PastieResponse (
		const Kakoune::SafePtr<SettingsBag>& parSettings,
		std::ostream* parStreamOut,
		const Kakoune::SafePtr<cgi::Env>& parCgiEnv
	) :
		GeneralPastieResponse(parSettings, parStreamOut, parCgiEnv),
		m_plain_text(false),
		m_syntax_highlight(true)
	{
	}

	tawashi::HttpHeader PastieResponse::on_general_pastie_process() {
		auto get = cgi_env().query_string_split();
		const std::string& query_str(cgi_env().query_string());
		if (get["m"] == "plain" or query_str.empty()) {
			m_plain_text = true;
			return tawashi::make_header_type_text_utf8();
		}
		else if (query_str == g_nolang_token) {
			m_syntax_highlight = false;
		}
		else {
			m_pastie_lang.clear();
			if (not query_str.empty())
				m_pastie_lang = query_str;
		}
		return tawashi::make_header_type_html();
	}

	void PastieResponse::on_general_mustache_prepare (std::string&& parPastie, mstch::map& parContext) {
		std::string processed_pastie;
		std::string highlight_comment;
		if (m_syntax_highlight) {
			processed_pastie = std::move(parPastie);
		}
		else {
			tawashi::Escapist houdini;
			processed_pastie = houdini.escape_html(parPastie);
		}

		if (not m_plain_text and m_syntax_highlight) {
			SplitHighlightedPastie split = highlight_string(std::move(processed_pastie), m_pastie_lang, settings());
			processed_pastie = std::move(split.text);
			highlight_comment = std::move(split.comment);
		}

		parContext["pastie"] = std::move(processed_pastie);
		parContext["pastie_lines"] = pastie_to_numbered_lines(
			boost::get<std::string>(parContext["pastie"])
		);
		parContext["highlight_comment"] = std::move(highlight_comment);
		parContext["colourless"] = not m_syntax_highlight;
	}

	std::string PastieResponse::on_mustache_retrieve() {
		if (m_plain_text)
			return "{{pastie}}";
		else
			return load_mustache();
	}
} //namespace kamokan
