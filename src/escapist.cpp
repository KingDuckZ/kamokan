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

#include "escapist.hpp"
#include "houdini.h"
#include <cstddef>
#include <cassert>

namespace tawashi {
	Escapist::Escapist() :
		m_gh_buf(new(&m_gh_buf_mem) gh_buf GH_BUF_INIT)
	{
		assert(reinterpret_cast<uintptr_t>(&m_gh_buf_mem) == reinterpret_cast<uintptr_t>(m_gh_buf));
		static_assert(sizeof(implem::DummyGHBuf) == sizeof(gh_buf), "Dummy struct has the wrong size");
		static_assert(sizeof(gh_buf) == sizeof(m_gh_buf_mem), "Static memory for gh_buf has the wrong size");
		static_assert(alignof(gh_buf) == alignof(m_gh_buf_mem), "Static memory for gh_buf has the wrong alignment");
	}

	Escapist::~Escapist() noexcept {
		gh_buf_free(static_cast<gh_buf*>(m_gh_buf));
		static_cast<gh_buf*>(m_gh_buf)->~gh_buf();
	}

	std::string Escapist::unescape_url (const boost::string_ref& parURL) const {
		if (parURL.empty())
			return std::string();

		assert(m_gh_buf);
		gh_buf* const buf = static_cast<gh_buf*>(m_gh_buf);

		const int escaped = houdini_unescape_url(
			buf,
			reinterpret_cast<const uint8_t*>(parURL.data()),
			parURL.size()
		);
		if (0 == escaped)
			return std::string(parURL.data(), parURL.size());
		else
			return std::string(buf->ptr, buf->size);
	}

	std::string Escapist::escape_html (const boost::string_ref& parHtml) const {
		if (parHtml.empty())
			return std::string();

		assert(m_gh_buf);
		gh_buf* const buf = static_cast<gh_buf*>(m_gh_buf);

		const int escaped = houdini_escape_html0(
			buf,
			reinterpret_cast<const uint8_t*>(parHtml.data()),
			parHtml.size(),
			1
		);
		if (0 == escaped)
			return std::string(parHtml.data(), parHtml.size());
		else
			return std::string(buf->ptr, buf->size);
	}
} //namespace tawashi
