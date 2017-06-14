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

#include "fake_storage.hpp"
#include "num_to_token.hpp"
#include <utility>
#include <algorithm>

namespace kamokan {
	FakeStorage::FakeStorage (const Kakoune::SafePtr<SettingsBag>& parSettings, bool parItsConnected) :
		Storage(parSettings),
		m_submission_num(1),
		m_its_connected(parItsConnected)
	{
	}

	FakeStorage::~FakeStorage() = default;

	void FakeStorage::connect_async() {
	}

	bool FakeStorage::is_connected() const {
		return m_its_connected;
	}

	void FakeStorage::finalize_connection() {
	}

	Storage::SubmissionResult FakeStorage::submit_pastie (
		const boost::string_view& parText,
		uint32_t parExpiry,
		const boost::string_view& parLang,
		bool parSelfDestruct,
		const std::string& parRemoteIP
	) const {
		SubmittedPastie pastie;
		std::string token = num_to_token(m_submission_num++);
		pastie.text = std::string(parText);
		pastie.expiry = parExpiry;
		pastie.lang = std::string(parLang);
		pastie.remote_ip = parRemoteIP;
		pastie.token = token;
		pastie.self_destruct = parSelfDestruct;
		m_submitted_pasties.push_back(std::move(pastie));

		Storage::SubmissionResult submission_res;
		submission_res.token = token;
		return submission_res;
	}

	boost::optional<std::string> FakeStorage::retrieve_pastie (const boost::string_view& parToken) const {
		auto it_found = std::find_if(
			m_submitted_pasties.begin(),
			m_submitted_pasties.end(),
			[&](const SubmittedPastie& pastie) {
				return pastie.token == parToken;
			}
		);
		if (m_submitted_pasties.end() == it_found)
			return boost::optional<std::string>();
		else
			return boost::make_optional(it_found->text);
	}

	const std::vector<FakeStorage::SubmittedPastie>& FakeStorage::submitted_pasties() const {
		return m_submitted_pasties;
	}
} //namespace kamokan
