#include "split_get_vars.hpp"
#include <boost/algorithm/string/finder.hpp>
#include <boost/utility/string_ref.hpp>
#include <boost/range/adaptor/transformed.hpp>
#include <boost/range/adaptor/filtered.hpp>
#include <ciso646>
#include <boost/algorithm/string/find_iterator.hpp>
#include <boost/range/iterator_range_core.hpp>
#include <boost/range/algorithm/find.hpp>

namespace tawashi {
	std::vector<std::pair<boost::string_ref, boost::string_ref>> split_env_vars (const std::string& parList) {
		using MatchRange = boost::iterator_range<std::string::const_iterator>;
		using boost::token_finder;
		using boost::adaptors::transformed;
		using boost::adaptors::filtered;
		using boost::string_ref;
		using boost::split_iterator;
		using boost::make_iterator_range;
		using boost::range::find;

		//See:
		//https://stackoverflow.com/questions/27999941/how-to-use-boostsplit-with-booststring-ref-in-boost-1-55
		//http://www.boost.org/doc/libs/1_60_0/doc/html/boost/algorithm/token_finder.html
		//https://stackoverflow.com/questions/20781090/difference-between-boostsplit-vs-boostiter-split
		return boost::copy_range<std::vector<std::pair<string_ref, string_ref>>>(
			make_iterator_range(
				split_iterator<std::string::const_iterator>(parList, token_finder([](char c){return '&'==c;})),
				split_iterator<std::string::const_iterator>()
			) |
			filtered([](const MatchRange& r){ return not r.empty(); }) |
			transformed([](const MatchRange& r){
				auto eq = find(r, '=');
				if (r.empty())
					return std::pair<string_ref, string_ref>();
				if (r.end() == eq)
					return std::make_pair(string_ref(&*r.begin(), r.size()), string_ref());
				else
					return std::make_pair(string_ref(&*r.begin(), eq - r.begin()), string_ref(&*(eq + 1), r.size() - (eq - r.begin() + 1)));
			})
		);
	}
} //namespace tawashi
