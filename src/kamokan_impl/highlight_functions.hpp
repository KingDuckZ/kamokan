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

#pragma once

#include <vector>
#include <string>
#include <boost/utility/string_view.hpp>
#include <string>

namespace kamokan {
	class SettingsBag;
	typedef std::vector<std::string> HighlightLangList;

	struct SplitHighlightedPastie {
		std::string comment;
		std::string text;
	};

	HighlightLangList list_highlight_langs (const SettingsBag& parSettings);
	SplitHighlightedPastie highlight_string (std::string&& parIn, const std::string& parLang, const SettingsBag& parSettings);
} //namespace kamokan
