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
#include "cgi_post.hpp"
#include "cgi_env.hpp"
#include "sanitized_utf8.hpp"
#include <vector>
#include <string>
#include <sstream>
#include <utility>
#include <iterator>
#include <fstream>
#include <ciso646>
#include <glib.h>

extern "C" {
	extern const unsigned char UTF_8_test_txt[];
	extern const unsigned int UTF_8_test_txt_len;
} //extern C

TEST_CASE ("Retrieve and sanitize invalid an invalid utf-8 text", "[utf8][security]") {
	using tawashi::cgi::PostMapType;

	auto content_length = std::string("CONTENT_LENGTH=") + std::to_string(UTF_8_test_txt_len);
	const std::string invalid_text_prefix("invalid_text=");
	std::string invalid_text;
	invalid_text.reserve(invalid_text_prefix.size() + UTF_8_test_txt_len);
	invalid_text = "invalid_text=";
	std::copy(reinterpret_cast<const char*>(UTF_8_test_txt), reinterpret_cast<const char*>(UTF_8_test_txt) + UTF_8_test_txt_len, std::back_inserter(invalid_text));

	std::istringstream iss;
	iss >> std::noskipws;
	iss.str(std::move(invalid_text));

	const char* const fake_env[] = {
		content_length.c_str(),
		nullptr
	};

	tawashi::cgi::Env env(fake_env);
	const PostMapType& post_data = read_post(iss, env);

	CHECK(g_utf8_validate(post_data.at("invalid_text").data(), post_data.at("invalid_text").size(), nullptr));

	//std::istringstream iss_expected;
	//iss_expected >> std::noskipws;
	//iss_expected.str(std::string(reinterpret_cast<const char*>(libreoffice_UTF_8_test_txt), libreoffice_UTF_8_test_txt_len));
	//std::string expected_line;
	//std::istringstream iss_obtained;
	//iss_obtained >> std::noskipws;
	//iss_obtained.str(post_data.at("invalid_text"));
	//for (std::string line; std::getline(iss_obtained, line); ) {
	//	std::getline(iss_expected, expected_line);
	//	std::cout << '"' << line << "\n\"" << expected_line << "\"\n";
	//	REQUIRE(line == expected_line);
	//}
}
