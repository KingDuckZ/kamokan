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
#include "mime_split.hpp"

TEST_CASE ("Test the Mime-type splitter", "[mime][parser]") {
	using tawashi::SplitMime;
	using tawashi::split_mime;

	bool ok;
	int parsed_count;
	{
		std::string test("application/x-javascript; charset=UTF-8");
		SplitMime split = split_mime(&test, ok, parsed_count);
		REQUIRE(ok);
		CHECK(test.size() == parsed_count);
		CHECK(split.type == "application");
		CHECK(split.subtype == "x-javascript");
		REQUIRE(split.parameters.size() == 1);
		CHECK(split.parameters.find("charset") != split.parameters.end());
		CHECK(split.parameters.at("charset") == "UTF-8");
	}

	{
		std::string test("image/jpeg; filename=genome.jpeg; modification-date=\"Wed, 12 Feb 1997 16:29:51 -0500\"");
	}
}
