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

#include "tawashi_exception.hpp"
#include <sstream>

namespace tawashi {
	namespace {
		std::string compose_err_message (ErrorReasons parReason, const boost::string_ref& parMessage) {
			std::ostringstream oss;
			oss << "Exception with reason " << parReason << ": " << parMessage;
			return oss.str();
		}
	} //unnamed namespace

	TawashiException::TawashiException (ErrorReasons parReason, const boost::string_ref& parMessage) :
		std::runtime_error(compose_err_message(parReason, parMessage)),
		m_reason(parReason)
	{
	}

	TawashiException::~TawashiException() noexcept = default;
} //namespace tawashi
