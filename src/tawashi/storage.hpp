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

#include "kakoune/safe_ptr.hh"
#include "error_reasons.hpp"
#include <memory>
#include <cstdint>
#include <boost/utility/string_ref.hpp>
#include <boost/optional.hpp>
#include <string>

namespace redis {
	class IncRedis;
} //namespace redis

namespace tawashi {
	class SettingsBag;

	class Storage {
	public:
		struct SubmissionResult {
			std::string token;
			boost::optional<ErrorReasons> error;
		};

		explicit Storage (const Kakoune::SafePtr<SettingsBag>& parSettings);
		~Storage();

		void connect_async();
		bool is_connected() const;
		void finalize_connection();
		SubmissionResult submit_pastie (
			const boost::string_ref& parText,
			uint32_t parExpiry,
			const boost::string_ref& parLang,
			const std::string& parRemoteIP
		) const;

		boost::optional<std::string> retrieve_pastie (const boost::string_ref& parToken) const;

	private:
		std::unique_ptr<redis::IncRedis> m_redis;
		Kakoune::SafePtr<SettingsBag> m_settings;
	};
} //namespace tawashi
