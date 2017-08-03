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
#include <srchilite/langmap.h>
#include <boost/range/adaptor/map.hpp>
#include <boost/range/algorithm/copy.hpp>
#include <boost/range/iterator_range_core.hpp>

namespace kamokan {
	HighlightLangList list_highlight_langs (const SettingsBag& parSettings) {
		srchilite::LangMap lang_map(parSettings.as<std::string>("langmap_dir"), "lang.map");
		lang_map.open();

		const auto lang_range = boost::make_iterator_range(lang_map.begin(), lang_map.end());
		return boost::copy_range<HighlightLangList>(lang_range | boost::adaptors::map_keys);
	}
} //namespace kamokan

