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

#include "sprout/array/array.hpp"
#include "sprout/cstring/strlen.hpp"
#include "duckhandy/sequence_bt.hpp"
#include <cstddef>
#include <cstdint>

namespace tawashi {
	typedef uint16_t string_length_type;

	namespace implem {
		template <std::size_t I, std::size_t S>
		inline constexpr string_length_type string_length_at_index (const sprout::array<const char*, S>& parStrings) {
			return static_cast<string_length_type>(sprout::strlen(parStrings[I]));
		}

		template <std::size_t... Indices>
		inline constexpr sprout::array<string_length_type, sizeof...(Indices)> string_lengths (const sprout::array<const char*, sizeof...(Indices)>& parStrings, dhandy::bt::index_seq<Indices...>) {
			return sprout::array<string_length_type, sizeof...(Indices)> {
				string_length_at_index<Indices>(parStrings)...
			};
		}

		template <typename Enum, std::size_t... Indices>
		inline constexpr sprout::array<string_length_type, Enum::_size()> string_lengths (const typename Enum::_name_iterable& parEnumIterable, dhandy::bt::index_seq<Indices...>) {
			return sprout::array<string_length_type, Enum::_size()> {
				static_cast<string_length_type>(sprout::strlen(parEnumIterable[Indices]))...
			};
		}
	} //namespace implem

	template <std::size_t S>
	inline constexpr sprout::array<string_length_type, S> string_lengths (const sprout::array<const char*, S>& parStrings) {
		return implem::string_lengths(parStrings, dhandy::bt::index_range<0, S>());
	}

	template <typename Enum>
	inline constexpr sprout::array<string_length_type, Enum::_size()> string_lengths() {
		return implem::string_lengths<Enum>(Enum::_names(), dhandy::bt::index_range<0, Enum::_size()>());
	}
} //namespace tawashi
