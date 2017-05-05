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
#include "settings_bag.hpp"
#include "ini_file.hpp"
#include "safe_stack_object.hpp"
#include <iterator>
#include <sstream>
#include <cstdint>
#include <spdlog/spdlog.h>

TEST_CASE ("Add and retrieve values from SettingsBag", "[settings][ini]") {
	using tawashi::SettingsBag;
	using tawashi::IniFile;
	using curry::SafeStackObject;

	auto statuslog = spdlog::stdout_logger_st("statuslog");

	std::stringstream ss;
	ss << std::noskipws;
	ss << "[tawashi]\n" <<
		"redis_server = 127.0.0.1\n" <<
		"empty=\n" <<
		"empty_spaces=     \n" <<
		"int_num= 42\n" <<
		"negative_int_num   = -5\n" <<
		"redis_mode = inet\n" <<
		"base_uri = http://127.0.0.1:8080\n"
	;
	ss.seekg(0);
	const int raw_ini_char_count = static_cast<int>(ss.str().size());
	REQUIRE(ss.tellg() == 0);

	auto ini = SafeStackObject<IniFile>(std::istream_iterator<char>(ss), std::istream_iterator<char>());
	REQUIRE(ini->parse_success());
	REQUIRE(ini->parsed_characters() == raw_ini_char_count);
	SettingsBag settings(ini);

	CHECK(settings["redis_server"] == "127.0.0.1");
	CHECK(settings["empty"].empty());
	CHECK(settings["empty_spaces"].empty());
	CHECK(settings.as<uint32_t>("int_num") == 42);
	CHECK(settings.as<std::string>("redis_mode") == "inet");
	CHECK(settings["base_uri"] == "http://127.0.0.1:8080");

	settings.add_default("redis_server", "192.168.0.5");
	settings.add_default("not_in_ini_empty", "");
	settings.add_default("not_in_ini", "abcd");
	settings.add_default("not_in_ini_num", "400");

	CHECK(settings["redis_server"] == "127.0.0.1");
	CHECK(settings["not_in_ini_empty"].empty());
	CHECK(settings["not_in_ini"] == "abcd");
	CHECK(settings.as<uint32_t>("not_in_ini_num") == 400);
}
