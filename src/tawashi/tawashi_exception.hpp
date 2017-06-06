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
#include "error_reasons.hpp"
#include <stdexcept>
#include <string>
#include <boost/utility/string_view.hpp>

namespace tawashi {
	class TawashiException : public std::runtime_error {
	public:
		TawashiException (ErrorReasons parReason, const boost::string_view& parMessage);
		~TawashiException() noexcept;

		ErrorReasons reason() const { return m_reason; }

	private:
		ErrorReasons m_reason;
	};
} //namespace tawashi
