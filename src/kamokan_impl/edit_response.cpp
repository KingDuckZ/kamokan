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

#include "edit_response.hpp"
#include "escapist.hpp"

namespace kamokan {
	EditResponse::EditResponse (
		const Kakoune::SafePtr<SettingsBag>& parSettings,
		std::ostream* parStreamOut,
		const Kakoune::SafePtr<cgi::Env>& parCgiEnv
	) :
		GeneralPastieResponse(parSettings, parStreamOut, parCgiEnv)
	{
	}

	tawashi::HttpHeader EditResponse::on_general_pastie_process() {
		return tawashi::make_header_type_html();
	}

	void EditResponse::on_general_mustache_prepare (std::string&& parPastie, mstch::map& parContext) {
		tawashi::Escapist houdini;
		parContext["pastie"] = houdini.escape_html(parPastie);
	}
} //namespace kamokan
