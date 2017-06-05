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
#include <boost/utility/string_ref.hpp>
#include <cassert>
#include <utility>

namespace tawashi {
	class SubmitPasteResponse : public Response {
	public:
		SubmitPasteResponse (
			const Kakoune::SafePtr<SettingsBag>& parSettings,
			std::ostream* parStreamOut,
			const Kakoune::SafePtr<cgi::Env>& parCgiEnv
		);

	protected:
		virtual boost::string_ref page_basename() const override { assert(false); return boost::string_ref(""); }
		virtual HttpHeader make_success_response (std::string&& parPastieParam);

	private:
		typedef std::pair<boost::optional<std::string>, HttpHeader> StringOrHeader;

		virtual HttpHeader on_process() override;
		StringOrHeader submit_to_storage (const boost::string_ref& parText, uint32_t parExpiry, const boost::string_ref& parLang);
	};
} //namespace tawashi
