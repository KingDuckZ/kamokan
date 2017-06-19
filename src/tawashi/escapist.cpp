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

//#define HTML_ESCAPE_WITH_HOUDINI

#include "escapist.hpp"
#include "houdini.h"
#include <cstddef>
#include <cassert>
#if !defined(HTML_ESCAPE_WITH_HOUDINI)
#	include <algorithm>
#endif

namespace tawashi {
	namespace {
#if !defined(HTML_ESCAPE_WITH_HOUDINI)
		[[gnu::pure]]
		uint32_t count_bits_set (uint32_t parVal) {
			auto& v = parVal;
			v = v - ((v >> 1) & 0x55555555); // reuse input as temporary
			v = (v & 0x33333333) + ((v >> 2) & 0x33333333); // temp
			return ((v + (v >> 4) & 0xF0F0F0F) * 0x1010101) >> 24; // count
		}
#endif

#if !defined(HTML_ESCAPE_WITH_HOUDINI)
		template <uint32_t I, char Haystack, char... HaystackRest>
		struct find_impl {
			static uint32_t find (char parNeedle) {
				return (Haystack == parNeedle ?
					I : find_impl<I + 1, HaystackRest...>::find(parNeedle)
				);
			}
		};
		template <uint32_t I, char Haystack>
		struct find_impl<I, Haystack> {
			static uint32_t find (char parNeedle) {
				return (Haystack == parNeedle ? I : I + 1);
			}
		};
		template <char... Haystack>
		[[gnu::pure,gnu::flatten]]
		uint32_t find (char parNeedle) {
			return find_impl<0, Haystack...>::find(parNeedle);
		}
#endif

#if !defined(HTML_ESCAPE_WITH_HOUDINI)
		template <char... Needle, std::size_t... Sizes>
		void slow_copy (const char* parSource, std::string& parDest, uint32_t parCount, const char (&...parWith)[Sizes]) {
			std::array<const char*, sizeof...(Needle)> withs {parWith...};
			std::array<uint32_t, sizeof...(Needle)> sizes {(static_cast<uint32_t>(Sizes) - 1)...};

			for (uint32_t z = 0; z < parCount; ++z) {
				const uint32_t match_index = find<Needle...>(parSource[z]);
				if (sizeof...(Needle) > match_index)
					parDest.append(withs[match_index], sizes[match_index]);
				else
					parDest.push_back(parSource[z]);
			}
		}
#endif

#if !defined(HTML_ESCAPE_WITH_HOUDINI)
		template <char... Needle, std::size_t... Sizes>
		std::string replace_with (const boost::string_view& parStr, const char (&...parWith)[Sizes]) {
			//Setup data
			static_assert(sizeof...(Needle) == sizeof...(Sizes), "Size mismatch");
			const std::array<uint32_t, sizeof...(Needle)> packs = {
				(compl static_cast<uint32_t>(0) / 0xFF * Needle)...
			};
			const std::array<char, sizeof...(Needle)> needles = { Needle... };
			const std::array<unsigned int, sizeof...(Needle)> sizes = {
				(static_cast<unsigned int>(Sizes) - 1)...
			};

			//Calculate the new string's size
			const std::size_t front_padding = (alignof(decltype(packs[0])) - reinterpret_cast<uintptr_t>(parStr.data()) % alignof(decltype(packs[0]))) % alignof(decltype(packs[0]));
			const unsigned int pre_bytes = std::min(front_padding, parStr.size());
			assert(pre_bytes < alignof(decltype(packs[0])));
			const unsigned int inp_size = static_cast<unsigned int>(parStr.size());
			const unsigned int mid_bytes = (inp_size - pre_bytes) - (inp_size - pre_bytes) % alignof(decltype(packs[0]));
			assert(0 == mid_bytes % alignof(decltype(packs[0])));
			unsigned int new_size = inp_size;
			unsigned int replace_count = 0;
			for (unsigned int z = 0; z < pre_bytes; ++z) {
				const auto needle_index = find<Needle...>(parStr[z]);
				if (sizeof...(Needle) > needle_index) {
					new_size += sizes[needle_index] - 1;
					++replace_count;
				}
			}

			assert(0 == (reinterpret_cast<uintptr_t>(parStr.data()) + pre_bytes) % alignof(decltype(packs[0])) or 0 == mid_bytes);
			const uint32_t c1 = 0x01010101UL;
			const uint32_t c2 = 0x80808080UL;
			assert(inp_size >= pre_bytes + mid_bytes);
			const unsigned int post_bytes = inp_size - pre_bytes - mid_bytes;
			assert(post_bytes < alignof(decltype(packs[0])));
			assert(post_bytes == (inp_size - pre_bytes) % alignof(decltype(packs[0])));
			assert(inp_size == pre_bytes + mid_bytes + post_bytes);
			for (unsigned int z = pre_bytes; z < inp_size - post_bytes; z += sizeof(packs[0])) {
				const uint32_t& val = *reinterpret_cast<const uint32_t*>(parStr.data() + z);
				for (unsigned int i = 0; i < sizeof...(Needle); ++i) {
					const uint32_t t = val xor packs[i];
					const uint32_t has_zero = (t - c1) bitand compl t bitand c2;
					new_size += (sizes[i] - 1) * count_bits_set(has_zero);
					replace_count += count_bits_set(has_zero);
				}
			}

			for (unsigned int z = inp_size - post_bytes; z < inp_size; ++z) {
				const auto needle_index = find<Needle...>(parStr[z]);
				if (sizeof...(Needle) > needle_index) {
					new_size += sizes[needle_index] - 1;
					++replace_count;
				}
			}

			if (not replace_count)
				return std::string(parStr);

			//Make the new string
			std::string retval;
			assert(new_size >= inp_size);
			retval.reserve(new_size);
			slow_copy<Needle...>(parStr.data(), retval, pre_bytes, parWith...);
			for (unsigned int z = pre_bytes; z < inp_size - post_bytes; z += sizeof(packs[0])) {
				const uint32_t& val = *reinterpret_cast<const uint32_t*>(parStr.data() + z);
				uint32_t escape_bytes = 0;
				for (uint32_t pack : packs) {
					const uint32_t t = val xor pack;
					escape_bytes = (t - c1) bitand compl t bitand c2;
					if (escape_bytes)
						break;
				}
				if (escape_bytes)
					slow_copy<Needle...>(parStr.data() + z, retval, sizeof(packs[0]), parWith...);
				else
					retval.append(parStr.data() + z, sizeof(packs[0]));
			}
			slow_copy<Needle...>(parStr.data() + inp_size - post_bytes, retval, post_bytes, parWith...);

			assert(new_size == retval.size());
			return retval;
		}
#endif
	} //unnamed namespace

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

	std::string Escapist::unescape_url (const boost::string_view& parURL) const {
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
			return std::string(parURL);
		else
			return std::string(buf->ptr, buf->size);
	}

	std::string Escapist::escape_url (const boost::string_view& parURL) const {
		if (parURL.empty())
			return std::string();

		assert(m_gh_buf);
		gh_buf* const buf = static_cast<gh_buf*>(m_gh_buf);

		const int escaped = houdini_escape_url(
			buf,
			reinterpret_cast<const uint8_t*>(parURL.data()),
			parURL.size()
		);
		if (0 == escaped)
			return std::string(parURL);
		else
			return std::string(buf->ptr, buf->size);
	}

	std::string Escapist::escape_html (const boost::string_view& parHtml) const {
		if (parHtml.empty())
			return std::string();

#if defined(HTML_ESCAPE_WITH_HOUDINI)
		assert(m_gh_buf);
		gh_buf* const buf = static_cast<gh_buf*>(m_gh_buf);

		const int escaped = houdini_escape_html0(
			buf,
			reinterpret_cast<const uint8_t*>(parHtml.data()),
			parHtml.size(),
			1
		);
		if (0 == escaped)
			return std::string(parHtml);
		else
			return std::string(buf->ptr, buf->size);
#else
		return replace_with<'&', '>', '<', '/', '"', '\''>(parHtml, "&amp;", "&gt;", "&lt;", "&#x2F;", "&quot;", "&#x27;");
#endif
	}
} //namespace tawashi
