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

#include "kamokan_config.h"
#include "submit_paste_response.hpp"
#include "quick_submit_paste_response.hpp"
#include "pastie_response.hpp"
#include "index_response.hpp"
#include "error_response.hpp"
#include "edit_response.hpp"
#include "response_factory.hpp"
#include "cgi_env.hpp"
#include "ini_file.hpp"
#include "safe_stack_object.hpp"
#include "pathname/pathname.hpp"
#include "duckhandy/compatibility.h"
#include "settings_bag.hpp"
#include "logging_levels.hpp"
#include "request_method_type.hpp"
#include <spdlog/spdlog.h>
#include <string>
#include <fstream>
#include <iterator>
#include <ciso646>
#include <iostream>
#include <chrono>

//www.open-std.org/jtc1/sc22/wg21/docs/papers/2014/n4150.pdf

namespace {
	std::string config_file_path() a_pure;

	std::string config_file_path() {
		mchlib::PathName config_path(KAMOKAN_CONFIG_PATH);
		mchlib::PathName full_path("");
		if (config_path.is_absolute()) {
			full_path = std::move(config_path);
		}
		else {
			full_path = mchlib::PathName(KAMOKAN_PATH_PREFIX);
			full_path.join(config_path);
		}
		full_path.join(KAMOKAN_CONFIG_FILE);
		return full_path.path();
	}

	template <typename T>
	std::unique_ptr<kamokan::Response> make_response (
		const Kakoune::SafePtr<kamokan::SettingsBag>& parSettings,
		const Kakoune::SafePtr<kamokan::cgi::Env>& parCgiEnv
	) {
		return static_cast<std::unique_ptr<kamokan::Response>>(
			std::make_unique<T>(parSettings, &std::cout, parCgiEnv)
		);
	}

	void fill_defaults (kamokan::SettingsBag& parSettings) {
		parSettings.add_default("redis_server", "127.0.0.1");
		parSettings.add_default("redis_port", "6379");
		parSettings.add_default("redis_mode", "sock");
		parSettings.add_default("redis_sock", "/tmp/redis.sock");
		parSettings.add_default("redis_db", "0");
		parSettings.add_default("host_name", "127.0.0.1");
		parSettings.add_default("host_port", "");
		parSettings.add_default("host_path", "/");
		parSettings.add_default("website_root", "");
		parSettings.add_default("langmap_dir", "/usr/share/source-highlight");
		parSettings.add_default("min_pastie_size", "10");
		parSettings.add_default("max_pastie_size", "10000");
		parSettings.add_default("truncate_long_pasties", "false");
		parSettings.add_default("logging_level", "err");
		parSettings.add_default("resubmit_wait", "10");
		parSettings.add_default("log_file", "-");
		parSettings.add_default("highlight_css", "sh_darkness.css");
		parSettings.add_default("max_post_size", "1048576");
		parSettings.add_default("max_token_length", "10");
	}

	void print_buildtime_info() {
		std::cout << "NDEBUG defined: ";
#if defined(NDEBUG)
		std::cout << "yes (Release build)";
#else
		std::cout << "no (Debug build)";
#endif
		std::cout << '\n';
		std::cout << "KAMOKAN_CONFIG_FILE: \"" << KAMOKAN_CONFIG_FILE << "\"\n";
		std::cout << "KAMOKAN_CONFIG_PATH: \"" << KAMOKAN_CONFIG_PATH << "\"\n";
		std::cout << "KAMOKAN_PATH_PREFIX: \"" << KAMOKAN_PATH_PREFIX << "\"\n";
		std::cout << "VERSION_MAJOR: " << VERSION_MAJOR << '\n';
		std::cout << "VERSION_MINOR: " << VERSION_MINOR << '\n';
		std::cout << "VERSION_PATCH: " << VERSION_PATCH << '\n';
		std::cout << "config_file_path(): \"" << config_file_path() << "\"\n";
	}

	curry::SafeStackObject<kamokan::IniFile> load_ini() {
		using curry::SafeStackObject;
		using kamokan::IniFile;
		using std::istream_iterator;

		std::ifstream conf(config_file_path());
		conf >> std::noskipws;
		return SafeStackObject<IniFile>(istream_iterator<char>(conf), istream_iterator<char>());
	}

	std::shared_ptr<spdlog::logger> setup_logging (const kamokan::SettingsBag& parSettings) {
		//Prepare the logger
		spdlog::set_pattern("[%Y-%m-%d %T %z] - %v");
		spdlog::set_level(spdlog::level::trace); //set to maximum possible here
		boost::string_view log_path = parSettings["log_file"];
		const bool log_to_stderr = (log_path == "-");
		auto statuslog = (log_to_stderr ?
			spdlog::stderr_logger_st("statuslog") :
			spdlog::basic_logger_st("statuslog", std::string(log_path.begin(), log_path.end()), false)
		);

		auto logging_level = tawashi::LoggingLevels::_from_string_nocase(parSettings.as<std::string>("logging_level").c_str());
		spdlog::set_level(static_cast<decltype(spdlog::level::trace)>(logging_level._to_integral()));
		return statuslog;
	}
} //unnamed namespace

int main (int parArgc, char* parArgv[], char* parEnvp[]) {
	using curry::SafeStackObject;
	using kamokan::IndexResponse;
	using kamokan::SubmitPasteResponse;
	using kamokan::QuickSubmitPasteResponse;
	using kamokan::PastieResponse;
	using kamokan::ErrorResponse;
	using kamokan::EditResponse;
	using kamokan::Response;
	using tawashi::RequestMethodType;

	std::chrono::time_point<std::chrono::steady_clock> app_start_time = std::chrono::steady_clock::now();

	if (2 == parArgc and boost::string_view(parArgv[1]) == "--show-paths") {
		print_buildtime_info();
		return 0;
	}

	SafeStackObject<kamokan::IniFile> ini = load_ini();
	auto settings = SafeStackObject<kamokan::SettingsBag>(ini, "kamokan");
	fill_defaults(*settings);

	auto statuslog = setup_logging(*settings);
	SPDLOG_DEBUG(statuslog, "kamokan started");
	int retval = 0;
	try {
		statuslog->info("Loaded config: \"{}\"", config_file_path());

		auto cgi_env = SafeStackObject<tawashi::cgi::Env>(parEnvp, settings->at("host_path"));
		kamokan::ResponseFactory resp_factory(settings, cgi_env);
		SPDLOG_TRACE(statuslog, "Registering makers in the response factory");
		resp_factory.register_maker("index.cgi", RequestMethodType::GET, &make_response<IndexResponse>);
		resp_factory.register_maker("", RequestMethodType::GET, &make_response<IndexResponse>);
		resp_factory.register_maker("", RequestMethodType::POST, &make_response<QuickSubmitPasteResponse>);
		resp_factory.register_maker("paste.cgi", RequestMethodType::POST, &make_response<SubmitPasteResponse>);
		resp_factory.register_maker("error.cgi", RequestMethodType::GET, &make_response<ErrorResponse>);
		resp_factory.register_maker("edit.cgi", RequestMethodType::GET, &make_response<EditResponse>);
		resp_factory.register_jolly_maker(&make_response<PastieResponse>, RequestMethodType::GET);
		resp_factory.set_app_start_time(app_start_time);

		std::unique_ptr<Response> response = resp_factory.make_response(
			tawashi::cgi::drop_arguments(cgi_env->request_uri_relative()),
			cgi_env->request_method()
		);
		if (response)
			response->send();
	}
	catch (const std::exception& e) {
		statuslog->critical("Uncaught exception in main(): \"{}\"", e.what());
		retval = 1;
	}

	SPDLOG_DEBUG(statuslog, "kamokan done, quitting with {}", retval);
	return retval;
}
