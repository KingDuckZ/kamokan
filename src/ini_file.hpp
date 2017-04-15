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
#include <boost/container/flat_map.hpp>
#include <iterator>
#include <boost/utility/string_ref.hpp>
#include <string>

namespace tawashi {
	class IniFile : public Kakoune::SafeCountable {
	public:
		typedef boost::container::flat_map<boost::string_ref, boost::string_ref> KeyValueMapType;
		typedef boost::container::flat_map<boost::string_ref, KeyValueMapType> IniMapType;

		IniFile (std::istream_iterator<char> parInputFrom, std::istream_iterator<char> parInputEnd);
		explicit IniFile (std::string&& parIniData);
		IniFile (IniFile&& parOther);
		IniFile (const IniFile& parOther) = delete;
		~IniFile() noexcept;

		IniFile& operator== (IniFile&&) = delete;
		IniFile& operator== (const IniFile&) = delete;

		const IniMapType& parsed() const { return m_map; }

	private:
		std::string m_raw_ini;
		IniMapType m_map;
	};
} //namespace tawashi
