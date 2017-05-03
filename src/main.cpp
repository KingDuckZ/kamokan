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

#include "tawashiConfig.h"
#include "submit_paste_response.hpp"
#include "pastie_response.hpp"
#include "index_response.hpp"
#include "response_factory.hpp"
#include "cgi_env.hpp"
#include "ini_file.hpp"
#include "safe_stack_object.hpp"
#include "pathname/pathname.hpp"
#include "duckhandy/compatibility.h"
#include "settings_bag.hpp"
#include <spdlog/spdlog.h>
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

	template <typename T>
	std::unique_ptr<tawashi::Response> make_response (const Kakoune::SafePtr<tawashi::SettingsBag>& parSettings) {
		return static_cast<std::unique_ptr<tawashi::Response>>(std::make_unique<T>(parSettings));
	}

	void fill_defaults (tawashi::SettingsBag& parSettings) {
		parSettings.add_default("redis_server", "127.0.0.1");
		parSettings.add_default("redis_port", "6379");
		parSettings.add_default("redis_mode", "sock");
		parSettings.add_default("redis_sock", "/tmp/redis.sock");
		parSettings.add_default("redis_db", "0");
		parSettings.add_default("base_uri", "http://127.0.0.1");
		parSettings.add_default("website_root", "");
		parSettings.add_default("langmap_dir", "/usr/share/source-highlight");
		parSettings.add_default("min_pastie_size", "10");
		parSettings.add_default("max_pastie_size", "10000");
		parSettings.add_default("truncate_long_pasties", "false");
	}
} //unnamed namespace

int main() {
	using curry::SafeStackObject;
	using tawashi::IndexResponse;
	using tawashi::SubmitPasteResponse;
	using tawashi::PastieResponse;
	using tawashi::Response;

	//Prepare the logger
	spdlog::set_pattern("[%Y-%m-%d %T %z] - %v");
	spdlog::set_level(spdlog::level::debug);
	auto statuslog = spdlog::stderr_logger_st("statuslog");

	statuslog->debug("Loading config: \"{}\"\n", config_file_path());

	std::ifstream conf(config_file_path());
	conf >> std::noskipws;
	auto ini = SafeStackObject<tawashi::IniFile>(std::istream_iterator<char>(conf), std::istream_iterator<char>());
	conf.close();

	auto settings = SafeStackObject<tawashi::SettingsBag>(ini);
	fill_defaults(*settings);

	tawashi::cgi::Env cgi_env;
	tawashi::ResponseFactory resp_factory(settings);
	resp_factory.register_maker("index.cgi", &make_response<IndexResponse>);
	resp_factory.register_maker("", &make_response<IndexResponse>);
	resp_factory.register_maker("paste.cgi", &make_response<SubmitPasteResponse>);
	resp_factory.register_jolly_maker(&make_response<PastieResponse>);

	std::unique_ptr<Response> response = resp_factory.make_response(cgi_env.path_info().substr(1));
	response->send();

	return 0;
}
