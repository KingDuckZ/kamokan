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
		Response(Response::ContentType, "text/html", "text", parSettings, true),
		m_langmap_dir(parSettings->as<std::string>("langmap_dir")),
		m_plain_text(false)
	{
	}

	void PastieResponse::on_process() {
		auto env = cgi_env().query_string_split();
		if (env["m"] == "plain" or cgi_env().query_string().empty()) {
			this->change_type(Response::ContentType, "text/plain");
			m_plain_text = true;
		}
		else {
			srchilite::LangMap lang_map(m_langmap_dir, "lang.map");
			lang_map.open();
			if (not cgi_env().query_string().empty())
				m_lang_file = lang_map.getFileName(cgi_env().query_string());
			else
				m_lang_file = "default.lang";
		}
	}

	void PastieResponse::on_mustache_prepare (mstch::map& parContext) {
		using opt_string = redis::IncRedis::opt_string;
		using opt_string_list = redis::IncRedis::opt_string_list;

		auto token = boost::string_ref(cgi_env().path_info()).substr(1);
		auto& redis = this->redis();
		opt_string_list pastie_reply = redis.hmget(token, "pastie");
		opt_string pastie = (pastie_reply and not pastie_reply->empty() ? (*pastie_reply)[0] : opt_string());

		srchilite::SourceHighlight highlighter;
		highlighter.setDataDir(settings().as<std::string>("langmap_dir"));
		highlighter.setGenerateEntireDoc(false);
		highlighter.setGenerateLineNumbers(true);
		const auto lang = m_lang_file;
		//Escapist houdini;
		//std::istringstream iss(houdini.escape_html(*pastie));

		if (m_plain_text) {
			parContext["pastie"] = *pastie;
		}
		else {
			std::istringstream iss(*pastie);
			std::ostringstream oss;
			highlighter.highlight(iss, oss, lang);
			parContext["pastie"] = oss.str();
		}
	}

	std::string PastieResponse::on_mustache_retrieve() {
		if (m_plain_text)
			return "{{pastie}}";
		else
			return load_mustache();
	}
} //namespace tawashi
