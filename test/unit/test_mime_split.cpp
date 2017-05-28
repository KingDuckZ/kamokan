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

namespace {
	std::string to_string (const boost::string_ref& parRef) {
		return std::string(parRef.data(), parRef.size());
	}
} //unnamed namespace

TEST_CASE ("Test the string_to_mime parser", "[mime][parser]") {
	using tawashi::SplitMime;
	using tawashi::string_to_mime;

	bool ok;
	int parsed_count;
	{
		std::string test("application/x-javascript; charset=UTF-8");
		std::string curr_val;
		SplitMime split = string_to_mime(&test, ok, parsed_count);

		REQUIRE(ok);
		CHECK(test.size() == parsed_count);
		CHECK(split.type == "application");
		CHECK(split.subtype == "x-javascript");
		REQUIRE(split.parameters.size() == 1);

		CHECK(split.parameters.find("charset") != split.parameters.end());
		curr_val = to_string(split.parameters.at("charset"));
		CHECK(curr_val == "UTF-8");
	}

	{
		std::string test("image/jpeg; filename=genome.jpeg; modification-date=\"Wed, 12 Feb 1997 16:29:51 -0500\"");
		std::string curr_val;
		SplitMime split = string_to_mime(&test, ok, parsed_count);

		REQUIRE(ok);
		CHECK(test.size() == parsed_count);
		CHECK(split.type == "image");
		CHECK(split.subtype == "jpeg");
		REQUIRE(split.parameters.size() == 2);

		CHECK(split.parameters.find("filename") != split.parameters.end());
		curr_val = to_string(split.parameters.at("filename"));
		CHECK(curr_val == "genome.jpeg");

		CHECK(split.parameters.find("modification-date") != split.parameters.end());
		curr_val = to_string(split.parameters.at("modification-date"));
		CHECK(curr_val == "Wed, 12 Feb 1997 16:29:51 -0500");
	}
}

TEST_CASE ("Test the mime_to_string parser", "[mime][parser]") {
	using tawashi::SplitMime;
	using tawashi::mime_to_string;

	std::string type = "image";
	std::string subtype = "jpeg";
	bool ok;

	{
		SplitMime test;
		test.type = type;
		test.subtype = subtype;
		std::string result = mime_to_string(test, ok);
		REQUIRE(ok);
		CHECK(result == "image/jpeg");
	}
	{
		SplitMime test;
		test.type = type;
		test.subtype = subtype;
		test.parameters["filename"] = "genome.jpeg";
		std::string result = mime_to_string(test, ok);
		REQUIRE(ok);
		CHECK(result == "image/jpeg; filename=genome.jpeg");
	}
	{
		SplitMime test;
		test.type = type;
		test.subtype = subtype;
		test.parameters["modification-date"] = "Wed, 12 Feb 1997 16:29:51 -0500";
		test.parameters["filename"] = "genome.jpeg";
		std::string result = mime_to_string(test, ok);
		REQUIRE(ok);
		CHECK(result == "image/jpeg; filename=genome.jpeg; modification-date=\"Wed, 12 Feb 1997 16:29:51 -0500\"");
	}
}
