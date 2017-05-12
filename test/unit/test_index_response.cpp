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
#include "fake_cgi_env.hpp"
#include "index_response.hpp"
#include "ini_file.hpp"
#include "settings_bag.hpp"
#include "safe_stack_object.hpp"
#include <sstream>
#include <utility>
#include <string>

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

	std::string env_ini(
		"[fake_env]\n"
		"  auth_type = \n"
		"  content_type = \n"
		"  path_info = \n"
		"  path_translated = \n"
		"  query_string = index.cgi\n"
		"  remote_addr = \n"
		"  remote_host = \n"
		"  remote_ident = \n"
		"  remote_user = \n"
		"  request_method = GET\n"
		"  script_name = \n"
		"  server_name = test_server\n"
		"  server_software = \n"
		"  content_length = 0\n"
		"  server_port = 80\n"
	);
	SafeStackObject<tawashi::cgi::FakeEnv> fake_env(std::move(env_ini));

	std::string tawashi_settings(
		"[tawashi]\n"
		"  base_uri = http://127.0.0.1\n"
		"  website_root = /home/michele/dev/code/cpp/tawashi/html\n"
		"  logging_level = debug\n"
		"  langmap_dir = /usr/share/source-highlight\n"
	);
	SafeStackObject<tawashi::IniFile> ini(std::move(tawashi_settings));
	SafeStackObject<tawashi::SettingsBag> settings(ini);

	std::stringstream response_stream;

	tawashi::IndexResponseCustomMustache response(settings, &response_stream, fake_env);
	response.send();

	response_stream.seekg(0);
	CHECK(response_stream.str() == g_expected_response);
}
