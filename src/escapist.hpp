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

#include <boost/utility/string_ref.hpp>
#include <string>
#include <memory>

namespace tawashi {
	namespace implem {
		//not actually used, only needed to get the size of the actual gh_buf
		//without including its full header file
		struct DummyGHBuf{
			char *ptr;
			size_t asize, size;
		};
	} //namespace implem

	class Escapist {
	public:
		Escapist();
		~Escapist() noexcept;

		std::string unescape_url (const boost::string_ref& parURL) const;
		std::string escape_html (const boost::string_ref& parHtml) const;

	private:
		std::aligned_storage<sizeof(implem::DummyGHBuf), alignof(implem::DummyGHBuf)>::type m_gh_buf_mem;
		void* m_gh_buf;
	};
} //namespace tawashi
