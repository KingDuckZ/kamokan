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

#include "response.hpp"
#include "kakoune/safe_ptr.hh"
#include "request_method_type.hpp"
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

	class ResponseFactory {
	public:
		typedef std::function<std::unique_ptr<Response>(const Kakoune::SafePtr<SettingsBag>&, const Kakoune::SafePtr<tawashi::cgi::Env>& parCgiEnv)> ResponseMakerFunc;

		explicit ResponseFactory (const Kakoune::SafePtr<SettingsBag>& parSettings, const Kakoune::SafePtr<tawashi::cgi::Env>& parCgiEnv);
		~ResponseFactory() noexcept;

		std::unique_ptr<Response> make_response(const boost::string_view& parName, tawashi::RequestMethodType parReqType);
		void register_maker (std::string&& parName, ResponseMakerFunc parMaker);
		void register_maker (std::string&& parName, tawashi::RequestMethodType parReqType, ResponseMakerFunc parMaker);
		void register_jolly_maker (ResponseMakerFunc parMaker, tawashi::RequestMethodType parReqType);
		void set_app_start_time (const std::chrono::time_point<std::chrono::steady_clock>& parTime);

	private:
		struct LocalData;

		std::unique_ptr<Response> make_response (ResponseMakerFunc parMaker) const;

		std::unique_ptr<LocalData> m_local_data;
	};
} //namespace kamokan
