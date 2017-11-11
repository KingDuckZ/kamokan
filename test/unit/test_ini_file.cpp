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
#include "ini_file.hpp"
#include <string>
#include <utility>
#include <ciso646>

TEST_CASE ("Test parsing an ini text", "[ini][parser]") {
	using kamokan::IniFile;

	//empty data
	{
		std::string empty;
		IniFile ini(std::move(empty));
		CHECK(ini.parse_success());
		CHECK(ini.parsed_characters() == 0);
		CHECK(ini.parsed().empty());
	}

	//valid data
	{
		std::string text(
			"\n"
			"[empty_section]\n"
			"[lololo]\n"
			"\n"
			"                \n"
			"\t\t   \t\n"
			"\tsample_key1\t   =   \t value 1\t\n"
			"sample_key2= \t value 2_overwritten     \n"
			"sample_key2  =value 2\n"
			" sample_key3=value 3\n"
			"\n"
			"sample_key4=\n"
			"sample_key5=     \t    \n"
			"\n"
			"  [ section 2 ] \n"
			"\tsect_2_val1=10\n"
			"\tsect_2_val2=20"
		);
		const int text_len = static_cast<int>(text.size());
		IniFile ini(std::move(text));

		CHECK(ini.parse_success());
		REQUIRE(ini.parsed_characters() == text_len);

		const IniFile::IniMapType& parsed = ini.parsed();
		CHECK(parsed.size() == 3);
		REQUIRE_NOTHROW(parsed.at("lololo"));
		REQUIRE_NOTHROW(parsed.at("section 2"));
		REQUIRE_NOTHROW(parsed.at("empty_section"));
		CHECK_THROWS(parsed.at("section3"));

		const IniFile::KeyValueMapType& lololo = parsed.at("lololo");
		REQUIRE(lololo.size() == 5);
		CHECK(lololo.at("sample_key1") == "value 1");
		CHECK(lololo.at("sample_key2") == "value 2_overwritten");
		CHECK(lololo.at("sample_key3") == "value 3");
		CHECK(lololo.at("sample_key4") == "");
		CHECK(lololo.at("sample_key5") == "");

		const IniFile::KeyValueMapType& empty_section = parsed.at("empty_section");
		CHECK(empty_section.empty());

		const IniFile::KeyValueMapType& section2 = parsed.at("section 2");
		REQUIRE(section2.size() == 2);
		CHECK(section2.at("sect_2_val1") == "10");
		CHECK(section2.at("sect_2_val2") == "20");
	}

	//invalid data
	{
		std::string text(
			"\n"
			"                \n"
			"\t\t   \t\n"
			"\tsample_key1\t   "
		);
		IniFile ini(std::move(text));
		CHECK(not ini.parse_success());
		//CHECK(ini.parsed_characters() == 27);
	}
}

TEST_CASE ("Test parsing an ini with comments", "[ini][parser][comments]") {
	using kamokan::IniFile;

	std::string text(
		"#leading comment\n"
		"[section]\n"
		"value = key\n"
		"#this = shoul be ignored\n"
		"\t#indented = comments should also be ignored\n"
		"this = #should not be ignored #lol\n"
		"#comment without newline"
	);
	const int text_len = static_cast<int>(text.size());
	IniFile ini(std::move(text));

	CHECK(ini.parse_success());
	REQUIRE(ini.parsed_characters() == text_len);

	const IniFile::IniMapType& parsed = ini.parsed();
	CHECK(parsed.size() == 1);
	REQUIRE_NOTHROW(parsed.at("section"));

	const IniFile::KeyValueMapType& section = parsed.at("section");
	REQUIRE(section.size() == 2);
	REQUIRE_NOTHROW(section.at("value"));
	REQUIRE_NOTHROW(section.at("this"));
	CHECK_THROWS(section.at("#this"));

	CHECK(section.at("value") == "key");
	CHECK(section.at("this") == "#should not be ignored #lol");
}
