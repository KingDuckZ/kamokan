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

#include "ini_file.hpp"
#include "kakoune/safe_ptr.hh"
#if defined(SPDLOG_DEBUG_ON)
#	include "spdlog.hpp"
#endif
#include <map>
#include <boost/utility/string_view.hpp>
#include <functional>
#include <string>

namespace kamokan {
	class SettingsBag : public Kakoune::SafeCountable {
		typedef std::map<boost::string_view, boost::string_view> MapType;
	public:
		SettingsBag (const Kakoune::SafePtr<IniFile>& parIni, boost::string_view parSectionName);
		~SettingsBag() noexcept;

		const boost::string_view& operator[] (boost::string_view parIndex) const;
		const boost::string_view& at (boost::string_view parIndex) const;
		template <typename T> T as (boost::string_view parIndex) const;
		void add_default (boost::string_view parKey, boost::string_view parValue);

	private:
		MapType m_defaults;
		Kakoune::SafePtr<IniFile> m_ini;
		const IniFile::KeyValueMapType* m_values;
	};

	template <>
	inline boost::string_view SettingsBag::as (boost::string_view parIndex) const {
		return (*this)[parIndex];
	}

	inline const boost::string_view& SettingsBag::at (boost::string_view parIndex) const {
#if defined(SPDLOG_DEBUG_ON)
		SPDLOG_DEBUG(spdlog::get("statuslog"), "Retrieving setting \"{}\"", parIndex);
#endif
		return (*this)[parIndex];
	}
} //namespace kamokan
