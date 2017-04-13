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

#include "incredis/incredis.hpp"
#include "tawashiConfig.h"
#include "submit_paste_response.hpp"
#include "pastie_response.hpp"
#include "index_response.hpp"
#include "cgi_env.hpp"
#include "ini_file.hpp"
#include "pathname/pathname.hpp"
#include "duckhandy/compatibility.h"
#include "duckhandy/lexical_cast.hpp"
#include <iostream>
#include <string>
#include <fstream>
#include <iterator>
#include <ciso646>

//www.open-std.org/jtc1/sc22/wg21/docs/papers/2014/n4150.pdf

namespace {
	std::string config_file_path() a_pure;

	std::string config_file_path() {
		mchlib::PathName config_path(TAWASHI_CONFIG_PATH);
		mchlib::PathName full_path("");
		if (config_path.is_absolute()) {
			full_path = std::move(config_path);
		}
		else {
			full_path = mchlib::PathName(TAWASHI_PATH_PREFIX);
			full_path.join(config_path);
		}
		full_path.join(TAWASHI_CONFIG_FILE);
		return full_path.path();
	}

	redis::IncRedis make_incredis (const tawashi::IniFile::KeyValueMapType& parSettings) {
		using redis::IncRedis;

		if (parSettings.at("redis_mode") == "inet") {
			return IncRedis(
				std::string(parSettings.at("redis_server")),
				dhandy::lexical_cast<uint16_t>(parSettings.at("redis_port"))
			);
		}
		else if (parSettings.at("redis_mode") == "sock") {
			return IncRedis(std::string(parSettings.at("redis_sock")));
		}
		else {
			throw std::runtime_error("Unknown setting for \"redis_mode\", valid settings are \"inet\" or \"sock\"");
		}
	}
} //unnamed namespace

int main() {
#if !defined(NDEBUG)
	std::cerr << "Loading config: \"" << config_file_path() << "\"\n";
#endif
	std::ifstream conf(config_file_path());
	conf >> std::noskipws;
	tawashi::IniFile ini = tawashi::IniFile(std::istream_iterator<char>(conf), std::istream_iterator<char>());
	conf.close();
	const auto& settings = ini.parsed().at("tawashi");

	auto incredis = make_incredis(settings);
	incredis.connect();

	tawashi::cgi::Env cgi_env;
	const boost::string_ref& base_uri = settings.at("base_uri");
	if (cgi_env.path_info() == "/index.cgi") {
		tawashi::IndexResponse resp(base_uri);
		resp.send();
	}
	else if (cgi_env.path_info() == "/paste.cgi") {
		tawashi::SubmitPasteResponse resp(incredis, base_uri);
		resp.send();
	}
	else {
		tawashi::PastieResponse resp(incredis, base_uri);
		resp.send();
	}

	return 0;
}
