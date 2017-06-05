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

#include "storage.hpp"
#include <vector>
#include <string>
#include <cstdint>

#if !defined(TAWASHI_WITH_TESTING)
#	error "Can't compile this class if test mode is disabled"
#endif

namespace tawashi {
	class FakeStorage : public Storage {
	public:
		struct SubmittedPastie {
			std::string text;
			std::string lang;
			std::string remote_ip;
			std::string token;
			uint32_t expiry;
		};

		FakeStorage (const Kakoune::SafePtr<SettingsBag>& parSettings, bool parItsConnected);
		tawashi_virtual_testing ~FakeStorage();

		tawashi_virtual_testing void connect_async();
		tawashi_virtual_testing bool is_connected() const;
		tawashi_virtual_testing void finalize_connection();
		tawashi_virtual_testing SubmissionResult submit_pastie (
			const boost::string_ref& parText,
			uint32_t parExpiry,
			const boost::string_ref& parLang,
			const std::string& parRemoteIP
		) const;

		tawashi_virtual_testing boost::optional<std::string> retrieve_pastie (const boost::string_ref& parToken) const;

		const std::vector<SubmittedPastie>& submitted_pasties() const;

	private:
		mutable std::vector<SubmittedPastie> m_submitted_pasties;
		mutable int64_t m_submission_num;
		bool m_its_connected;
	};
} //namespace tawashi
