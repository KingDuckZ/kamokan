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

#include "highlight_functions.hpp"
#include "settings_bag.hpp"
#include <srchilite/sourcehighlight.h>
#include <srchilite/langmap.h>
#include <boost/range/adaptor/map.hpp>
#include <boost/range/algorithm/copy.hpp>
#include <boost/range/iterator_range_core.hpp>
#include <sstream>

namespace kamokan {
	namespace {
		std::string highlight_css_path (const SettingsBag& parSettings) {
			//TODO: make sure the file exists or throw or do something
			return parSettings.as<std::string>("highlight_css");
		}

		std::string lang_name_to_file_path (const std::string& parLang, const std::string& parLangmapDir) {
			if (parLang.empty())
				return std::string();

			srchilite::LangMap lang_map(parLangmapDir, "lang.map");
			lang_map.open();
			std::string lang_file = lang_map.getFileName(parLang);
			if (lang_file.empty())
				lang_file = "default.lang";

			return lang_file;
		}

		SplitHighlightedPastie strip_tags_from_highlighted (std::string parPastie) {
			//I'm expecting some comment at the beginning, like:
			//		<!-- Generator: GNU source-highlight 3.1.8
			//		by Lorenzo Bettini
			//		http://www.lorenzobettini.it
			//		http://www.gnu.org/software/src-highlite -->

			SplitHighlightedPastie retval;
			{
				const auto comment_start = parPastie.find("<!--");
				if (parPastie.npos != comment_start) {
					const auto comment_len = parPastie.find("-->") - comment_start + 3;
					retval.comment = parPastie.substr(comment_start, comment_len);
					const std::size_t newline = (comment_len + 1 < parPastie.size() and parPastie[comment_len] == '\n' ? 1 : 0);
					parPastie.erase(comment_start, comment_len + newline);
				}
			}

			{
				const auto pre_start = parPastie.find("<pre><tt>");
				if (parPastie.npos != pre_start) {
					parPastie.erase(pre_start, 9);
				}
			}

			{
				const auto pre_cl_start = parPastie.find("</tt></pre>");
				if (parPastie.npos != pre_cl_start) {
					parPastie.erase(pre_cl_start, 11);
				}
			}

			retval.text = std::move(parPastie);
			return retval;
		}
	} //unnamed namespace

	HighlightLangList list_highlight_langs (const SettingsBag& parSettings) {
		srchilite::LangMap lang_map(parSettings.as<std::string>("langmap_dir"), "lang.map");
		lang_map.open();

		const auto lang_range = boost::make_iterator_range(lang_map.begin(), lang_map.end());
		return boost::copy_range<HighlightLangList>(lang_range | boost::adaptors::map_keys);
	}

	SplitHighlightedPastie highlight_string (std::string&& parIn, const std::string& parLang, const SettingsBag& parSettings) {
		const std::string langmap_dir = parSettings.as<std::string>("langmap_dir");
		srchilite::SourceHighlight highlighter;
		highlighter.setDataDir(langmap_dir);
		highlighter.setGenerateEntireDoc(false);
		highlighter.setGenerateLineNumbers(true);
#if defined(NDEBUG)
		highlighter.setOptimize(true);
#else
		highlighter.setOptimize(false);
#endif
		highlighter.setCanUseStdOut(false);
		highlighter.setTabSpaces(4);
		highlighter.setStyleCssFile(highlight_css_path(parSettings));
		highlighter.setGenerateLineNumbers(false);

		std::istringstream iss(std::move(parIn));
		std::ostringstream oss;
		highlighter.highlight(iss, oss, lang_name_to_file_path(parLang, langmap_dir));
		return strip_tags_from_highlighted(oss.str());
	}
} //namespace kamokan

