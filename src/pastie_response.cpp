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
#include "incredis/incredis.hpp"
#include "settings_bag.hpp"
#include <ciso646>
#include <srchilite/sourcehighlight.h>
#include <srchilite/langmap.h>
#include <sstream>

namespace tawashi {
	PastieResponse::PastieResponse (const Kakoune::SafePtr<SettingsBag>& parSettings) :
		Response(Response::ContentType, "text/html", "", parSettings, true),
		m_plain_text(false)
	{
	}

	void PastieResponse::on_process() {
		auto env = cgi_env().query_string_split();
		if (env["m"] == "plain") {
			this->change_type(Response::ContentType, "text/plain");
			m_plain_text = true;
		}
		else {
			srchilite::LangMap lang_map("/usr/share/source-highlight", "lang.map");
			lang_map.open();
			if (not cgi_env().query_string().empty())
				m_lang_file = lang_map.getFileName(cgi_env().query_string());
			else
				m_lang_file = "default.lang";
		}
	}

	void PastieResponse::on_send (std::ostream& parStream) {
		using opt_string = redis::IncRedis::opt_string;

		if (cgi_env().path_info().empty()) {
			return;
		}

		auto token = boost::string_ref(cgi_env().path_info()).substr(1);
		auto& redis = this->redis();
		opt_string pastie = redis.get(token);
		if (not pastie) {
			assert(false);
		}

		if (m_plain_text) {
			parStream << *pastie;
		}
		else {
			srchilite::SourceHighlight highlighter;
			highlighter.setDataDir("/usr/share/source-highlight");
			highlighter.setGenerateEntireDoc(false);
			highlighter.setGenerateLineNumbers(true);
			const auto lang = m_lang_file;
			std::istringstream iss(*pastie);

			highlighter.highlight(iss, parStream, lang);
		}
	}
} //namespace tawashi
