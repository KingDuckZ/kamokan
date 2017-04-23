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

#include "settings_bag.hpp"
#include <cassert>

namespace tawashi {
	SettingsBag::SettingsBag (const Kakoune::SafePtr<IniFile>& parIni) :
		m_ini(parIni),
		m_values(&parIni->parsed().at("tawashi"))
	{
		assert(m_values);
	}

	SettingsBag::~SettingsBag() noexcept = default;

	const boost::string_ref& SettingsBag::operator[] (boost::string_ref parIndex) const {
		auto it_found = m_values->find(parIndex);
		if (m_values->end() != it_found)
			return it_found->second;
		else
			return m_defaults.at(parIndex);
	}

	const boost::string_ref& SettingsBag::as_ref (boost::string_ref parIndex) const {
		return (*this)[parIndex];
	}

	std::string SettingsBag::as_str (boost::string_ref parIndex) const {
		auto& setting = (*this)[parIndex];
		return std::string(setting.data(), setting.size());
	}

	void SettingsBag::add_default (boost::string_ref parKey, boost::string_ref parValue) {
		assert(m_defaults.find(parKey) == m_defaults.end());
		m_defaults[parKey] = parValue;
	}
} //namespace tawashi

