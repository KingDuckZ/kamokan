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

#include "list_highlight_langs.hpp"
#include <srchilite/langmap.h>
#include <boost/range/adaptor/map.hpp>
#include <boost/range/algorithm/copy.hpp>
#include <boost/range/iterator_range_core.hpp>

namespace tawashi {
	HighlightLangList list_highlight_langs() {
		const char langmap_path[] = "/usr/share/source-highlight";
		srchilite::LangMap lang_map(langmap_path, "lang.map");
		lang_map.open();

		const auto lang_range = boost::make_iterator_range(lang_map.begin(), lang_map.end());
		return boost::copy_range<HighlightLangList>(lang_range | boost::adaptors::map_keys);
	}
} //namespace tawashi

