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

#include "response.hpp"
#include <utility>

namespace tawashi {
	Response::Response (std::string&& parType) :
		m_content_type(std::move(parType))
	{
	}

	Response::~Response() noexcept = default;

	void Response::send() {
		std::cout << "Content-type:" << m_content_type << "\n\n";
		this->on_send(std::cout);
		std::cout.flush();
	}

	const cgi::Env& Response::cgi_env() const {
		return m_cgi_env;
	}
} //namespace tawashi
