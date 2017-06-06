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

#include "split_get_vars.hpp"
#include <boost/algorithm/string/finder.hpp>
#include <boost/utility/string_view.hpp>
#include <boost/range/adaptor/transformed.hpp>
#include <boost/range/adaptor/filtered.hpp>
#include <ciso646>
#include <boost/algorithm/string/find_iterator.hpp>
#include <boost/range/iterator_range_core.hpp>
#include <boost/range/algorithm/find.hpp>

namespace tawashi {
	RawKeyValueList split_env_vars (const std::string& parList) {
		using MatchRange = boost::iterator_range<std::string::const_iterator>;
		using boost::token_finder;
		using boost::adaptors::transformed;
		using boost::adaptors::filtered;
		using boost::string_view;
		using boost::split_iterator;
		using boost::make_iterator_range;
		using boost::range::find;

		//See:
		//https://stackoverflow.com/questions/27999941/how-to-use-boostsplit-with-booststring-ref-in-boost-1-55
		//http://www.boost.org/doc/libs/1_60_0/doc/html/boost/algorithm/token_finder.html
		//https://stackoverflow.com/questions/20781090/difference-between-boostsplit-vs-boostiter-split
		return boost::copy_range<RawKeyValueList>(
			make_iterator_range(
				split_iterator<std::string::const_iterator>(parList, token_finder([](char c){return '&'==c;})),
				split_iterator<std::string::const_iterator>()
			) |
			filtered([](const MatchRange& r){ return not r.empty(); }) |
			transformed([](const MatchRange& r){
				auto eq = find(r, '=');
				if (r.empty())
					return std::pair<string_view, string_view>();
				if (r.end() == eq)
					return std::make_pair(string_view(&*r.begin(), r.size()), string_view());
				else
					return std::make_pair(string_view(&*r.begin(), eq - r.begin()), string_view(&*(eq + 1), r.size() - (eq - r.begin() + 1)));
			})
		);
	}
} //namespace tawashi
