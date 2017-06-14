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
#include <ciso646>
#include <srchilite/sourcehighlight.h>
#include <srchilite/langmap.h>
#include <sstream>
#include <algorithm>
#include <cassert>
#include <boost/algorithm/string/find_iterator.hpp>
#include <boost/range/iterator_range_core.hpp>
#include <boost/algorithm/string/finder.hpp>
#include <boost/range/adaptor/transformed.hpp>

namespace kamokan {
	namespace {
		const char g_nolang_token[] = "colourless";

		std::string highlight_css_path (const SettingsBag& parSettings) {
			//TODO: make sure the file exists or throw or do something
			return parSettings.as<std::string>("highlight_css");
		}

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

	PastieResponse::PastieResponse (
		const Kakoune::SafePtr<SettingsBag>& parSettings,
		std::ostream* parStreamOut,
		const Kakoune::SafePtr<cgi::Env>& parCgiEnv
	) :
		Response(parSettings, parStreamOut, parCgiEnv, true),
		m_langmap_dir(parSettings->as<std::string>("langmap_dir")),
		m_plain_text(false),
		m_syntax_highlight(true),
		m_pastie_not_found(false),
		m_token_invalid(false)
	{
	}

	tawashi::HttpHeader PastieResponse::on_process() {
		using tawashi::ErrorReasons;

		if (m_pastie_not_found)
			return make_error_redirect(ErrorReasons::PastieNotFound);
		if (m_token_invalid)
			return make_error_redirect(ErrorReasons::InvalidToken);

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
			srchilite::LangMap lang_map(m_langmap_dir, "lang.map");
			lang_map.open();
			m_lang_file.clear();
			if (not query_str.empty())
				m_lang_file = lang_map.getFileName(query_str);
			if (m_lang_file.empty())
				m_lang_file = "default.lang";
		}
		return tawashi::make_header_type_html();
	}

	void PastieResponse::on_mustache_prepare (mstch::map& parContext) {
		boost::string_view token = cgi::drop_arguments(cgi_env().request_uri_relative());
		Storage::RetrievedPastie pastie_info = this->storage().retrieve_pastie(token);

		if (not is_valid_token(token, settings().as<uint32_t>("max_token_length"))) {
			m_token_invalid = true;
			return;
		}
		if (not pastie_info.pastie) {
			m_pastie_not_found = true;
			return;
		}

		srchilite::SourceHighlight highlighter;
		highlighter.setDataDir(settings().as<std::string>("langmap_dir"));
		highlighter.setGenerateEntireDoc(false);
		highlighter.setGenerateLineNumbers(true);
#if defined(NDEBUG)
		highlighter.setOptimize(true);
#else
		highlighter.setOptimize(false);
#endif
		highlighter.setCanUseStdOut(false);
		highlighter.setTabSpaces(4);
		highlighter.setStyleCssFile(highlight_css_path(settings()));
		highlighter.setGenerateLineNumbers(false);

		std::string processed_pastie;
		if (m_syntax_highlight) {
			//TODO: redirect to "pastie not found" if !pastie
			processed_pastie = std::move(*pastie_info.pastie);
		}
		else {
			tawashi::Escapist houdini;
			std::ostringstream oss;
			oss << R"(<pre><tt><font color="#EDEDED">)";
			oss << houdini.escape_html(*pastie_info.pastie) << "</font></tt></pre>\n";
			processed_pastie = oss.str();
		}

		if (not m_plain_text and m_syntax_highlight) {
			std::istringstream iss(std::move(processed_pastie));
			std::ostringstream oss;
			highlighter.highlight(iss, oss, m_lang_file);
			processed_pastie = oss.str();
		}

		parContext["pastie"] = std::move(processed_pastie);
		parContext["pastie_lines"] = pastie_to_numbered_lines(
			boost::get<std::string>(parContext["pastie"])
		);
		parContext["self_destructed"] = pastie_info.self_destructed;
		parContext["pastie_token"] = token;
		parContext["pastie_page"] = true;
	}

	std::string PastieResponse::on_mustache_retrieve() {
		if (m_plain_text)
			return "{{pastie}}";
		else
			return load_mustache();
	}
} //namespace kamokan
