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
#include <string>
#include <boost/optional.hpp>

namespace tawashi {
	class SubmitPasteResponse : public Response {
	public:
		explicit SubmitPasteResponse (const IniFile& parIni);

	private:
		virtual void on_process() override;
		virtual void on_send (std::ostream& parStream) override;
		boost::optional<std::string> submit_to_redis (const std::string& parText) const;

		std::string m_error_message;
	};
} //namespace tawashi
