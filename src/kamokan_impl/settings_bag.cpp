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

#include "settings_bag.hpp"
#include "duckhandy/lexical_cast.hpp"
#include "spdlog.hpp"
#include <ciso646>
#include <cassert>
#include <cstdint>
#include <sstream>
#include <iostream>

namespace kamokan {
	namespace {
		const IniFile::KeyValueMapType* get_kamokan_node (const IniFile& parIni, boost::string_view parSectionName) {
			auto it_found = parIni.parsed().find(parSectionName);
			if (parIni.parsed().end() != it_found) {
				return &it_found->second;
			}
			else {
				std::cerr << "Couldn't find section [" << parSectionName << "] in the settings file\n";
				static const IniFile::KeyValueMapType empty_key_values;
				return &empty_key_values;
			}
		}
	} //unnamed namespace

	SettingsBag::SettingsBag (const Kakoune::SafePtr<IniFile>& parIni, boost::string_view parSectionName) :
		m_ini(parIni),
		m_values(get_kamokan_node(*parIni, parSectionName))
	{
		assert(m_values);
	}

	SettingsBag::~SettingsBag() noexcept = default;

	const boost::string_view& SettingsBag::operator[] (boost::string_view parIndex) const {
		auto it_found = m_values->find(parIndex);
		if (m_values->end() != it_found)
			return it_found->second;
		else
			return m_defaults.at(parIndex);
	}

	void SettingsBag::add_default (boost::string_view parKey, boost::string_view parValue) {
		assert(m_defaults.find(parKey) == m_defaults.end());
		m_defaults[parKey] = parValue;
	}

	template <>
	std::string SettingsBag::as (boost::string_view parIndex) const {
		auto& setting = this->at(parIndex);
		return std::string(setting);
	}

	template <>
	bool SettingsBag::as (boost::string_view parIndex) const {
		auto& setting = this->at(parIndex);
		if (setting == "true" or setting == "yes" or setting == "1" or setting == "on") {
			return true;
		}
		else if (setting == "false" or setting == "no" or setting == "0" or setting == "off") {
			return false;
		}
		else {
			std::ostringstream oss;
			oss << "Bad conversion: can't convert \"" << setting << "\" to bool";
			throw std::runtime_error(oss.str());
		}
	}

	template <>
	uint16_t SettingsBag::as (boost::string_view parIndex) const {
		return dhandy::lexical_cast<uint16_t>(this->at(parIndex));
	}

	template <>
	uint32_t SettingsBag::as (boost::string_view parIndex) const {
		return dhandy::lexical_cast<uint32_t>(this->at(parIndex));
	}

	template std::string SettingsBag::as<std::string> (boost::string_view parIndex) const;
	template bool SettingsBag::as<bool> (boost::string_view parIndex) const;
	template uint16_t SettingsBag::as<uint16_t> (boost::string_view parIndex) const;
	template uint32_t SettingsBag::as<uint32_t> (boost::string_view parIndex) const;
} //namespace kamokan
