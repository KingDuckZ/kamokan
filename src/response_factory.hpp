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

#include "response.hpp"
#include "kakoune/safe_ptr.hh"
#include <memory>

namespace tawashi {
	class SettingsBag;

	class ResponseFactory {
	public:
		typedef std::function<std::unique_ptr<Response>(const SettingsBag&)> ResponseMakerFunc;

		explicit ResponseFactory (const Kakoune::SafePtr<SettingsBag>& parSettings);
		~ResponseFactory() noexcept;

		std::unique_ptr<Response> make_response(const boost::string_ref& parName);
		void register_maker (std::string&& parName, ResponseMakerFunc parMaker);
		void register_jolly_maker (ResponseMakerFunc parMaker);

	private:
		struct LocalData;

		std::unique_ptr<LocalData> m_local_data;
	};
} //namespace tawashi
