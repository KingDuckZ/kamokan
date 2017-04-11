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
#include <cassert>

namespace tawashi {
	Response::Response (Types parRespType, std::string&& parValue, const boost::string_ref& parBaseURI) :
		m_resp_value(std::move(parValue)),
		m_base_uri(parBaseURI),
		m_resp_type(parRespType),
		m_header_sent(false)
	{
	}

	Response::~Response() noexcept = default;

	void Response::on_process() {
	}

	void Response::send() {
		this->on_process();

		m_header_sent = true;
		switch (m_resp_type) {
		case ContentType:
			std::cout << "Content-type: " << m_resp_value << "\n\n";
			break;
		case Location:
			std::cout << "Location: " << m_resp_value << "\n\n";
			break;
		}

		if (ContentType == m_resp_type)
			this->on_send(std::cout);
		std::cout.flush();
	}

	const cgi::Env& Response::cgi_env() const {
		return m_cgi_env;
	}

	void Response::change_type (Types parRespType, std::string&& parValue) {
		assert(not m_header_sent);
		assert(not parValue.empty());
		m_resp_type = parRespType;
		m_resp_value = std::move(parValue);
	}

	const boost::string_ref& Response::base_uri() const {
		return m_base_uri;
	}
} //namespace tawashi
