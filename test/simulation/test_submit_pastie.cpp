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
#include "submit_paste_response.hpp"
#include "escapist.hpp"
#include "simulation_config.h"
#include "ini_file.hpp"
#include "settings_bag.hpp"
#include "safe_stack_object.hpp"
#include "cgi_env.hpp"
#include "cgi_post.hpp"
#include "fake_storage.hpp"
#include <spdlog/spdlog.h>
#include <sstream>
#include <boost/lexical_cast.hpp>

namespace tawashi {
	class SubmitPasteResponseWithFakeStorage : public SubmitPasteResponse {
	public:
		SubmitPasteResponseWithFakeStorage (
			const Kakoune::SafePtr<SettingsBag>& parSettings,
			std::ostream* parStreamOut,
			const Kakoune::SafePtr<cgi::Env>& parCgiEnv,
			std::string&& parPostData
		) :
			SubmitPasteResponse(parSettings, parStreamOut, parCgiEnv, false),
			m_fake_storage(parSettings, true),
			m_post_data(std::move(parPostData))
		{
		}

		virtual const cgi::PostMapType& cgi_post() const override {
			CHECK(not m_post_data.empty());
			std::istringstream iss(m_post_data);
			return cgi::read_post(iss, this->cgi_env(), this->settings().as<uint32_t>("max_post_size"));
		}

		virtual const Storage& storage() const override {
			return m_fake_storage;
		}

		const std::vector<FakeStorage::SubmittedPastie>& submitted_pasties() const {
			return m_fake_storage.submitted_pasties();
		}

	private:
		FakeStorage m_fake_storage;
		std::string m_post_data;
	};
} //namespace tawashi

TEST_CASE ("Submit paste response", "[submitpaste][response]") {
	using curry::SafeStackObject;
	using tawashi::Escapist;

	Escapist houdini;

	std::string original_pastie = u8"不法投棄（ふほうとうき）とは、廃棄物の処理及び清掃に関する法律（主に、廃棄物処理法、廃掃法と略される）に違反して、同法に定めた処分場以外(主に山中や海、廃墟など人目につかない場所)に廃棄物を投棄することをいう。";
	std::string original_lang = u8"colourless";
	std::string original_expiry = "2017";

	std::string post_data =
			"pastie=" + houdini.escape_url(original_pastie) + "&" +
			"lang=" + houdini.escape_url(original_lang) + "&" +
			"ttl=" + houdini.escape_url(original_expiry)
	;

	std::string content_length_entry = "CONTENT_LENGTH=" + std::to_string(post_data.size());

	const char* const env_raw[] = {
		"AUTH_TYPE=",
		"CONTENT_TYPE=application/x-www-form-urlencoded; charset=utf-8",
		"QUERY_STRING=index.cgi",
		"REMOTE_ADDR=127.0.0.1",
		"REMOTE_HOST=",
		"REMOTE_IDENT=",
		"REMOTE_USER=",
		"REQUEST_URI=/kamokan/paste.cgi",
		"REQUEST_METHOD=POST"
		"SCRIPT_NAME=",
		"SERVER_NAME=test_server"
		"SERVER_SOFTWARE=UnitTest",
		content_length_entry.c_str(),
		"SERVER_PORT=80",
		"HTTPS=",
		"REQUEST_METHOD=POST",
		nullptr
	};
	SafeStackObject<tawashi::cgi::Env> fake_env(env_raw, "/");

	std::string kamokan_settings(
		"[kamokan]\n"
		"  host_name = 192.168.0.10\n"
		"  website_root = " KAMOKAN_HTML_PATH "\n"
		"  logging_level = debug\n"
		"  langmap_dir = /usr/share/source-highlight\n"
		"  host_path = /kamokan\n"
		"  host_port =\n"
		"  redis_mode = sock\n"
		"  redis_server = 1.2.3.4\n"
		"  redis_port = 6379\n"
		"  redis_sock = /lol/redis.sock\n"
		"  redis_db = 0\n"
		"  max_post_size = 1000000\n"
		"  max_pastie_size = 1500\n"
		"  min_pastie_size = 1\n"
		"  truncate_long_pasties = no\n"
	);
	SafeStackObject<tawashi::IniFile> ini(std::move(kamokan_settings));
	SafeStackObject<tawashi::SettingsBag> settings(ini, "kamokan");

	std::ostringstream oss;
	tawashi::SubmitPasteResponseWithFakeStorage submit_response(
		settings,
		&oss,
		fake_env,
		std::move(post_data)
	);

	submit_response.send();
	auto& submitted_pasties = submit_response.submitted_pasties();
	REQUIRE(submitted_pasties.size() == 1);
	auto& submitted_pastie = submitted_pasties.front();
	CHECK(submitted_pastie.text == original_pastie);
	CHECK(submitted_pastie.lang == original_lang);
#if defined(TAWASHI_WITH_IP_LOGGING)
	CHECK(submitted_pastie.remote_ip == "127.0.0.1");
#endif
	CHECK(submitted_pastie.token == "b");
	CHECK(submitted_pastie.expiry == boost::lexical_cast<uint32_t>(original_expiry));

	{
		std::string output = oss.str();
		const char expected[] = "Status: HTTP/1.1 303 See Other\n"
			"Location: http://192.168.0.10/kamokan/b?colourless\n\n"
		;
		CHECK(output == expected);
	}
}
