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
#include <boost/optional.hpp>
#include <boost/utility/string_view.hpp>
#include <cassert>
#include <utility>

namespace kamokan {
	class SubmitPasteResponse : public Response {
	public:
#if defined(KAMOKAN_WITH_TESTING)
		SubmitPasteResponse (
			const Kakoune::SafePtr<SettingsBag>& parSettings,
			std::ostream* parStreamOut,
			const Kakoune::SafePtr<cgi::Env>& parCgiEnv,
			bool parInitStorage
		);
#endif

		SubmitPasteResponse (
			const Kakoune::SafePtr<SettingsBag>& parSettings,
			std::ostream* parStreamOut,
			const Kakoune::SafePtr<cgi::Env>& parCgiEnv
		);

	protected:
		virtual boost::string_view page_basename() const override { return boost::string_view("saved"); }
		virtual tawashi::HttpHeader make_success_response (std::string&& parPastieParam);

	private:
		typedef std::pair<boost::optional<std::string>, tawashi::HttpHeader> StringOrHeader;

		virtual tawashi::HttpHeader on_process() override;
		virtual void on_mustache_prepare (mstch::map& parContext) override;
		StringOrHeader submit_to_storage (
			const boost::string_view& parText,
			uint32_t parExpiry,
			const boost::string_view& parLang,
			bool parSelfDestruct
		);

		std::string m_pastie_token;
	};
} //namespace kamokan
