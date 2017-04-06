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

#include "cgi_env.hpp"
#include <string>
#include <iostream>

namespace tawashi {
	class Response {
	public:
		virtual ~Response() noexcept;

		void send();

	protected:
		Response (std::string&& parType);
		const cgi::Env& cgi_env() const;

	private:
		virtual void on_send (std::ostream& parStream) = 0;

		cgi::Env m_cgi_env;
		std::string m_content_type;
	};
} //namespace tawashi
