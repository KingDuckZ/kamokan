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

#include "catch.hpp"
#include "escapist.hpp"
#include <utility>
#include <vector>
#include <string>
#include <boost/utility/string_view.hpp>

TEST_CASE ("Test html escaping", "[security][escape]") {
	using boost::string_view;
	using StrPair = std::pair<string_view, string_view>;

	const std::vector<StrPair> test_data {
		{"", ""},
		{"a", "a"},
		{"&", "&amp;"},
		{">", "&gt;"},
		{"<", "&lt;"},
		{"/", "&#x2F;"},
		{"\"", "&quot;"},
		{"'", "&#x27;"},
		{">a", "&gt;a"},
		{"a>", "a&gt;"},
		{"abcd", "abcd"},
		{"abcdefgh", "abcdefgh"},
		{"abcdefghi", "abcdefghi"},
		{"abcdefgh&", "abcdefgh&amp;"},
		{"ab&defghi", "ab&amp;defghi"},
		{"<>&123''", "&lt;&gt;&amp;123&#x27;&#x27;"},
		{"</body>", "&lt;&#x2F;body&gt;"},
		{"&\"lol\"&", "&amp;&quot;lol&quot;&amp;"}
	};

	tawashi::Escapist esc;
	for (const auto& p : test_data) {
		const auto& in = p.first;
		const auto& expected = p.second;
		std::string out = esc.escape_html(in);
		CHECK(out == expected);
	}
}
