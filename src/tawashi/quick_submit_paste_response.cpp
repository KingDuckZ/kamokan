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

#include "quick_submit_paste_response.hpp"

namespace tawashi {
	QuickSubmitPasteResponse::QuickSubmitPasteResponse (
		const Kakoune::SafePtr<SettingsBag>& parSettings,
		std::ostream* parStreamOut,
		const Kakoune::SafePtr<cgi::Env>& parCgiEnv
	) :
		SubmitPasteResponse(parSettings, parStreamOut, parCgiEnv)
	{
	}

	void QuickSubmitPasteResponse::on_mustache_prepare (mstch::map& parContext) {
		parContext["redirect_to_address"] = m_redirect_to;
	}

	std::string QuickSubmitPasteResponse::on_mustache_retrieve() {
		return "{{base_uri}}/{{redirect_to_address}}\n";
	}

	HttpHeader QuickSubmitPasteResponse::make_success_response (std::string&& parPastieParam) {
		m_redirect_to = std::move(parPastieParam);
		return HttpHeader();
	}
} //namespace tawashi
