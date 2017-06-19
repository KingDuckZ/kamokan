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

#pragma once

#include "mstch/mstch.hpp"
#include "kakoune/safe_ptr.hh"
#include "http_header.hpp"
#include "error_reasons.hpp"
#include "storage.hpp"
#include "cgi_post.hpp"
#include <string>
#include <iostream>
#include <boost/utility/string_view.hpp>
#include <memory>
#include <chrono>

namespace tawashi {
	namespace cgi {
		class Env;
	} //namespace cgi
} //namespace tawashi

namespace kamokan {
	class SettingsBag;
	namespace cgi = tawashi::cgi;

	class Response {
	public:
		virtual ~Response() noexcept;

		void send();
		void set_app_start_time (const std::chrono::time_point<std::chrono::steady_clock>& parTime);

	protected:
		Response (
			const Kakoune::SafePtr<SettingsBag>& parSettings,
			std::ostream* parStreamOut,
			const Kakoune::SafePtr<cgi::Env>& parCgiEnv,
			bool parWantRedis
		);

		const cgi::Env& cgi_env() const;
		kamokan_virtual_testing const cgi::PostMapType& cgi_post() const;

		const std::string& base_uri() const;
		virtual boost::string_view page_basename() const = 0;
		kamokan_virtual_testing const Storage& storage() const;
		const SettingsBag& settings() const;
		virtual std::string load_mustache() const;
		tawashi::HttpHeader make_redirect (tawashi::HttpStatusCodes parCode, const std::string& parLocation);
		tawashi::HttpHeader make_error_redirect (tawashi::ErrorReasons parReason);

	private:
		virtual tawashi::HttpHeader on_process();
		virtual void on_mustache_prepare (mstch::map& parContext);
		virtual std::string on_mustache_retrieve();
		virtual std::string default_pastie_lang();
		virtual bool is_submit_page() const { return false; }
		virtual bool is_pastie_page() const { return false; }

		Storage m_storage;
		Kakoune::SafePtr<cgi::Env> m_cgi_env;
		Kakoune::SafePtr<SettingsBag> m_settings;
		std::chrono::time_point<std::chrono::steady_clock> m_time0;
		std::string m_website_root;
		std::string m_base_uri;
		std::ostream* m_stream_out;
	};
} //namespace kamokan
