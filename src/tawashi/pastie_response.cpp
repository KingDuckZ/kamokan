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

#include "pastie_response.hpp"
#include "storage.hpp"
#include "settings_bag.hpp"
#include "escapist.hpp"
#include "cgi_env.hpp"
#include <ciso646>
#include <srchilite/sourcehighlight.h>
#include <srchilite/langmap.h>
#include <sstream>

namespace tawashi {
	namespace {
		const char g_nolang_token[] = "colourless";

		std::string highlight_css_path (const SettingsBag& parSettings) {
			//TODO: make sure the file exists or throw or do something
			return parSettings.as<std::string>("highlight_css");
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
		m_pastie_not_found(false)
	{
	}

	HttpHeader PastieResponse::on_process() {
		if (m_pastie_not_found) {
			return make_error_redirect(ErrorReasons::PastieNotFound);
		}

		auto get = cgi_env().query_string_split();
		const std::string& query_str(cgi_env().query_string());
		if (get["m"] == "plain" or query_str.empty()) {
			m_plain_text = true;
			return make_header_type_text_utf8();
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
		return make_header_type_html();
	}

	void PastieResponse::on_mustache_prepare (mstch::map& parContext) {
		boost::string_ref token = cgi_env().path_info_relative();
		boost::optional<std::string> pastie = this->storage().retrieve_pastie(token);

		if (not pastie) {
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
			processed_pastie = std::move(*pastie);
		}
		else {
			Escapist houdini;
			std::ostringstream oss;
			oss << R"(<pre><tt><font color="#EDEDED">)";
			oss << houdini.escape_html(*pastie) << "</font></tt></pre>\n";
			processed_pastie = oss.str();
		}

		if (not m_plain_text and m_syntax_highlight) {
			std::istringstream iss(std::move(processed_pastie));
			std::ostringstream oss;
			highlighter.highlight(iss, oss, m_lang_file);
			processed_pastie = oss.str();
		}

		parContext["pastie"] = std::move(processed_pastie);
	}

	std::string PastieResponse::on_mustache_retrieve() {
		if (m_plain_text)
			return "{{pastie}}";
		else
			return load_mustache();
	}
} //namespace tawashi
