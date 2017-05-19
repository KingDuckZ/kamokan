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
#include "cgi_env.hpp"
#include <cstdint>
#include <glib.h>
#include <ciso646>

TEST_CASE ("Retrieve and sanitize invalid an invalid utf-8 text from environment variables", "[utf8][security]") {
	const uint8_t path_info_valid[] = {
		'P', 'A', 'T', 'H', '_', 'I', 'N', 'F', 'O', '=',
		'/', 0xce, 0xba, 0xe1, 0xbd, 0xb9, 0xcf, 0x83, 0xce, 0xbc, 0xce, 0xb5,
		'\0'
	};
	const uint8_t path_translated_invalid[] = {
		'P', 'A', 'T', 'H', '_', 'T', 'R', 'A', 'N', 'S', 'L', 'A', 'T', 'E', 'D', '=',
		0xf8, 0x88, 0x80, 0x80, 0x80,
		'\0'
	};

	const char* const fake_env[] = {
		reinterpret_cast<const char*>(path_info_valid),
		reinterpret_cast<const char*>(path_translated_invalid),
		nullptr
	};

	tawashi::cgi::Env env(fake_env, "/");

	CHECK(g_utf8_validate(reinterpret_cast<const char*>(path_info_valid), sizeof(path_info_valid) - 1, nullptr));
	CHECK(g_utf8_validate(env.path_info().data(), env.path_info().size(), nullptr));
	CHECK(not g_utf8_validate(reinterpret_cast<const char*>(path_translated_invalid), sizeof(path_translated_invalid) - 1, nullptr));
	CHECK(g_utf8_validate(env.path_translated().data(), env.path_translated().size(), nullptr));
}
