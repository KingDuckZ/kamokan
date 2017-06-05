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

#include "catch.hpp"
#include "cgi_env.hpp"
#include "index_response.hpp"
#include "ini_file.hpp"
#include "settings_bag.hpp"
#include "safe_stack_object.hpp"
#include "simulation_config.h"
#include <sstream>
#include <utility>
#include <string>
#include <spdlog/spdlog.h>

namespace {
	const char g_mustache_html[] =
R"(<body>
	<form action="{{base_uri}}/paste.cgi" method="post" accept-charset="UTF-8">

	<div id="content">
		<textarea type="text" id="PasteTextBox" name="pastie" placeholder=">Text..." autofocus required autocomplete="off" name="text"></textarea>
	</div>

</form>
</body>
)";

	const char g_expected_response[] =
R"(Content-type: text/html

<body>
	<form action="http://127.0.0.1/paste.cgi" method="post" accept-charset="UTF-8">

	<div id="content">
		<textarea type="text" id="PasteTextBox" name="pastie" placeholder=">Text..." autofocus required autocomplete="off" name="text"></textarea>
	</div>

</form>
</body>
)";
} //unnamed namespace

namespace tawashi {
	class IndexResponseCustomMustache : public IndexResponse {
	public:
		IndexResponseCustomMustache (
			const Kakoune::SafePtr<SettingsBag>& parSettings,
			std::ostream* parStreamOut,
			const Kakoune::SafePtr<cgi::Env>& parCgiEnv
		) :
			IndexResponse(parSettings, parStreamOut, parCgiEnv)
		{
		}

		virtual std::string on_mustache_retrieve() override {
			return g_mustache_html;
		}
	};
} //namespace tawashi

TEST_CASE ("Index response", "[index][response]") {
	using curry::SafeStackObject;

	auto statuslog = spdlog::stdout_logger_st("statuslog");

	const char* const env_raw[] = {
		"AUTH_TYPE=",
		"CONTENT_TYPE=",
		"PATH_INFO=/",
		"PATH_TRANSLATED=",
		"QUERY_STRING=index.cgi"
		"REMOTE_ADDR=",
		"REMOTE_HOST=",
		"REMOTE_IDENT=",
		"REMOTE_USER=",
		"REQUEST_METHOD=GET"
		"SCRIPT_NAME=",
		"SERVER_NAME=test_server"
		"SERVER_SOFTWARE=",
		"CONTENT_LENGTH=",
		"SERVER_PORT=80",
		"HTTPS=",
		"REQUEST_METHOD=GET",
		nullptr
	};
	SafeStackObject<tawashi::cgi::Env> fake_env(env_raw, "/");

	std::string kamokan_settings(
		"[kamokan]\n"
		"  host_name = 127.0.0.1\n"
		"  website_root = " KAMOKAN_HTML_PATH "\n"
		"  logging_level = debug\n"
		"  langmap_dir = /usr/share/source-highlight\n"
		"  host_path = /\n"
		"  host_port =\n"
	);
	SafeStackObject<tawashi::IniFile> ini(std::move(kamokan_settings));
	SafeStackObject<tawashi::SettingsBag> settings(ini, "kamokan");

	std::stringstream response_stream;

	tawashi::IndexResponseCustomMustache response(settings, &response_stream, fake_env);
	response.send();

	response_stream.seekg(0);
	CHECK(response_stream.str() == g_expected_response);
}
