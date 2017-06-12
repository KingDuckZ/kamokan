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

#pragma once

#include "response.hpp"
#include <string>
#include <boost/utility/string_view.hpp>

namespace kamokan {
	class PastieResponse : public Response {
	public:
		PastieResponse (
			const Kakoune::SafePtr<SettingsBag>& parSettings,
			std::ostream* parStreamOut,
			const Kakoune::SafePtr<cgi::Env>& parCgiEnv
		);

	protected:
		virtual boost::string_view page_basename() const override { return boost::string_view("pastie"); }

	private:
		virtual tawashi::HttpHeader on_process() override;
		virtual void on_mustache_prepare (mstch::map& parContext) override;
		virtual std::string on_mustache_retrieve() override;

		std::string m_lang_file;
		std::string m_langmap_dir;
		bool m_plain_text;
		bool m_syntax_highlight;
		bool m_pastie_not_found;
		bool m_token_invalid;
	};
} //namespace kamokan
