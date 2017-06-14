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

namespace kamokan {
	class FakeStorage : public Storage {
	public:
		struct SubmittedPastie {
			std::string text;
			std::string lang;
			std::string remote_ip;
			std::string token;
			uint32_t expiry;
			bool self_destruct;
		};

		FakeStorage (const Kakoune::SafePtr<SettingsBag>& parSettings, bool parItsConnected);
		kamokan_virtual_testing ~FakeStorage();

		kamokan_virtual_testing void connect_async() override;
		kamokan_virtual_testing bool is_connected() const override;
		kamokan_virtual_testing void finalize_connection() override;
		kamokan_virtual_testing SubmissionResult submit_pastie (
			const boost::string_view& parText,
			uint32_t parExpiry,
			const boost::string_view& parLang,
			bool parSelfDestruct,
			const std::string& parRemoteIP
		) const override;

		kamokan_virtual_testing Storage::RetrievedPastie retrieve_pastie (const boost::string_view& parToken) const override;

		const std::vector<SubmittedPastie>& submitted_pasties() const;

	private:
		mutable std::vector<SubmittedPastie> m_submitted_pasties;
		mutable int64_t m_submission_num;
		bool m_its_connected;
	};
} //namespace kamokan
