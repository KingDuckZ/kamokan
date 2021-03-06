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

#include "kakoune/safe_ptr.hh"
#include "error_reasons.hpp"
#include <memory>
#include <cstdint>
#include <boost/utility/string_view.hpp>
#include <boost/optional.hpp>
#include <string>
#include <utility>

namespace redis {
	class IncRedis;
} //namespace redis

namespace kamokan {
	class SettingsBag;

	class Storage {
	public:
		struct SubmissionResult {
			std::string token;
			boost::optional<tawashi::ErrorReasons> error;
		};
		struct RetrievedPastie {
			RetrievedPastie();
			RetrievedPastie (boost::optional<std::string>&& parPastie, bool parSelfDestructed, bool parValidToken);
			~RetrievedPastie() = default;

			boost::optional<std::string> pastie;
			boost::optional<std::string> lang;
			boost::optional<std::string> error;
			boost::optional<std::string> comment;
			bool highlighted;
			bool self_destructed;
			bool valid_token;
		};

		explicit Storage (const Kakoune::SafePtr<SettingsBag>& parSettings);
		kamokan_virtual_testing ~Storage();

		kamokan_virtual_testing void connect_async();
		kamokan_virtual_testing bool is_connected() const;
		kamokan_virtual_testing void finalize_connection();
		kamokan_virtual_testing SubmissionResult submit_pastie (
			const boost::string_view& parText,
			uint32_t parExpiry,
			const boost::string_view& parLang,
			bool parSelfDestruct,
			const std::string& parRemoteIP
		) const;
		kamokan_virtual_testing void submit_highlighted_pastie (
			const boost::string_view& parToken,
			const boost::string_view& parText,
			const boost::string_view& parComment,
			uint32_t parMaxTokenLen
		) const;

		kamokan_virtual_testing RetrievedPastie retrieve_pastie (
			const boost::string_view& parToken,
			uint32_t parMaxTokenLen,
			const boost::string_view& parRequestedLang
		) const;

#if defined(KAMOKAN_WITH_TESTING)
		const SettingsBag& settings() const;
#endif

	private:
		std::unique_ptr<redis::IncRedis> m_redis;
		Kakoune::SafePtr<SettingsBag> m_settings;
	};

	inline Storage::RetrievedPastie::RetrievedPastie (
		boost::optional<std::string>&& parPastie,
		bool parSelfDestructed,
		bool parValidToken
	) :
		pastie(std::move(parPastie)),
		self_destructed(parSelfDestructed),
		valid_token(parValidToken)
	{
	}
} //namespace kamokan
