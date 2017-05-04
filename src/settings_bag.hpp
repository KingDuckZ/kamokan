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

#include "ini_file.hpp"
#include "kakoune/safe_ptr.hh"
#include <map>
#include <boost/utility/string_ref.hpp>
#include <functional>
#include <string>
#if defined(SPDLOG_DEBUG_ON)
#	include <spdlog/spdlog.h>
#endif

namespace tawashi {
	class SettingsBag : public Kakoune::SafeCountable {
		typedef std::map<boost::string_ref, boost::string_ref> MapType;
	public:
		explicit SettingsBag (const Kakoune::SafePtr<IniFile>& parIni);
		~SettingsBag() noexcept;

		const boost::string_ref& operator[] (boost::string_ref parIndex) const;
		const boost::string_ref& at (boost::string_ref parIndex) const;
		template <typename T> T as (boost::string_ref parIndex) const;
		void add_default (boost::string_ref parKey, boost::string_ref parValue);

	private:
		MapType m_defaults;
		Kakoune::SafePtr<IniFile> m_ini;
		const IniFile::KeyValueMapType* m_values;
	};

	template <>
	inline boost::string_ref SettingsBag::as (boost::string_ref parIndex) const {
		return (*this)[parIndex];
	}

	inline const boost::string_ref& SettingsBag::at (boost::string_ref parIndex) const {
		SPDLOG_DEBUG(spdlog::get("statuslog"), "Retrieving setting \"{}\"", std::string(parIndex.data(), parIndex.size()));
		return (*this)[parIndex];
	}
} //namespace tawashi
