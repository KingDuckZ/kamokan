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

#pragma once

#include "mstch/mstch.hpp"
#include "kakoune/safe_ptr.hh"
#include "http_header.hpp"
#include "error_reasons.hpp"
#include "storage.hpp"
#include <string>
#include <iostream>
#include <boost/utility/string_ref.hpp>
#include <memory>

namespace tawashi {
	class SettingsBag;

	namespace cgi {
		class Env;
	} //namespace cgi

	class Response {
	public:
		virtual ~Response() noexcept;

		void send();

	protected:
		Response (
			const Kakoune::SafePtr<SettingsBag>& parSettings,
			std::ostream* parStreamOut,
			const Kakoune::SafePtr<cgi::Env>& parCgiEnv,
			bool parWantRedis
		);

		const cgi::Env& cgi_env() const;
		const std::string& base_uri() const;
		virtual boost::string_ref page_basename() const = 0;
		const Storage& storage() const;
		const SettingsBag& settings() const;
		virtual std::string load_mustache() const;
		HttpHeader make_redirect (HttpStatusCodes parCode, const std::string& parLocation);
		HttpHeader make_error_redirect (ErrorReasons parReason);

	private:
		virtual HttpHeader on_process();
		virtual void on_mustache_prepare (mstch::map& parContext);
		virtual std::string on_mustache_retrieve();

		Storage m_storage;
		Kakoune::SafePtr<cgi::Env> m_cgi_env;
		Kakoune::SafePtr<SettingsBag> m_settings;
		std::string m_website_root;
		std::string m_base_uri;
		std::ostream* m_stream_out;
	};
} //namespace tawashi