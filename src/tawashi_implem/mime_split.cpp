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

#include "mime_split.hpp"
#include <boost/spirit/include/qi_core.hpp>
#include <boost/spirit/include/qi_parse.hpp>
#include <boost/spirit/include/qi_plus.hpp>
#include <boost/spirit/include/qi_lit.hpp>
#include <boost/spirit/include/qi_char_.hpp>
#include <boost/spirit/include/qi_raw.hpp>
#include <boost/spirit/include/qi_grammar.hpp>
#include <boost/spirit/include/qi_lexeme.hpp>
#include <boost/spirit/include/qi_char_.hpp>
#include <boost/spirit/include/qi_char_class.hpp>
#include <boost/spirit/include/qi_kleene.hpp>
#include <boost/spirit/include/qi_alternative.hpp>
#include <boost/spirit/include/qi_difference.hpp>

#include <boost/spirit/include/karma_char.hpp>
#include <boost/spirit/include/karma_generate.hpp>
#include <boost/spirit/include/karma_operator.hpp>
#include <boost/spirit/include/karma_kleene.hpp>
#include <boost/spirit/include/karma_stream.hpp>
#include <boost/spirit/include/karma_string.hpp>
#include <boost/spirit/include/karma_alternative.hpp>
#include <boost/spirit/include/karma_rule.hpp>
#include <boost/spirit/include/karma_eps.hpp>

#include <boost/fusion/include/std_pair.hpp>
#include <boost/fusion/include/adapt_struct.hpp>

#include <boost/phoenix/function/lazy_prelude.hpp>
#include <boost/phoenix/core.hpp>
#include <boost/phoenix/object/construct.hpp>
#include <boost/phoenix/stl/container.hpp>
#include <boost/phoenix/bind/bind_member_function.hpp>
#include <boost/phoenix/operator/arithmetic.hpp>
#include <boost/phoenix/operator/self.hpp>
#include <boost/phoenix/stl/algorithm/transformation.hpp>

#include <cassert>
#include <iterator>
#include <sstream>

//    The Internet Media Type [9 <#ref-9>] of the attached entity. The syntax is
//    the same as the HTTP Content-Type header.
//
//       CONTENT_TYPE = "" | media-type
//       media-type   = type "/" subtype *( ";" parameter)
//       type         = token
//       subtype      = token
//       parameter    = attribute "=" value
//       attribute    = token
//       value        = token | quoted-string
//
//    The type, subtype and parameter attribute names are not
//    case-sensitive. Parameter values may be case sensitive.  Media
//    types and their use in HTTP are described section 3.6 <#section-3.6> of the
//    HTTP/1.0 specification [3 <#ref-3>]. Example:
//
//       application/x-www-form-urlencoded
//
//    There is no default value for this variable. If and only if it is
//    unset, then the script may attempt to determine the media type
//    from the data received. If the type remains unknown, then
//    application/octet-stream should be assumed.

BOOST_FUSION_ADAPT_STRUCT(
	tawashi::SplitMime,
	(boost::string_ref, type)
	(boost::string_ref, subtype)
	(tawashi::MimeParametersMapType, parameters)
);

namespace tawashi {
	namespace {
		template <typename Iterator, typename Skipper>
		struct MimeGrammar : boost::spirit::qi::grammar<Iterator, tawashi::SplitMime(), Skipper> {
			explicit MimeGrammar (const std::string* parString);

			boost::spirit::qi::rule<Iterator, SplitMime(), Skipper> content_type;
			boost::spirit::qi::rule<Iterator, SplitMime(), Skipper> media_type;
			boost::spirit::qi::rule<Iterator, boost::string_ref(), Skipper> type;
			boost::spirit::qi::rule<Iterator, boost::string_ref(), Skipper> subtype;
			boost::spirit::qi::rule<Iterator, MimeParametersMapType::value_type(), Skipper> parameter;
			boost::spirit::qi::rule<Iterator, boost::string_ref(), Skipper> attribute;
			boost::spirit::qi::rule<Iterator, boost::string_ref(), Skipper> value;
			boost::spirit::qi::rule<Iterator, boost::string_ref(), Skipper> quoted_string;
			boost::spirit::qi::rule<Iterator, boost::string_ref(), Skipper> token;
			const std::string* m_master_string;
			Iterator m_begin;
		};

		template <typename Iterator, typename Skipper>
		MimeGrammar<Iterator, Skipper>::MimeGrammar (const std::string* parString) :
			MimeGrammar::base_type(content_type),
			m_master_string(parString),
			m_begin(m_master_string->cbegin())
		{
			namespace px = boost::phoenix;
			using boost::spirit::ascii::space;
			using boost::spirit::qi::char_;
			using boost::spirit::qi::lit;
			using boost::spirit::qi::alnum;
			using boost::spirit::qi::raw;
			using boost::spirit::qi::_val;
			using boost::spirit::qi::lexeme;
			using boost::string_ref;
			using boost::spirit::_1;

			content_type = -media_type;
			media_type = type >> "/" >> subtype >> *(lit(";") >> parameter);
			type = token.alias();
			subtype = token.alias();
			parameter = attribute >> "=" >> value;
			attribute = token.alias();
			value = token | quoted_string;

			token = raw[+(alnum | char_("_.-"))][
				_val = px::bind(
					&string_ref::substr, px::construct<string_ref>(px::ref(*m_master_string)),
					px::begin(_1) - px::ref(m_begin),
					px::size(_1)
				)
			];
			quoted_string = raw[
				lexeme[
					lit('"') >>
					*(char_ - '"') >>
					'"'
				]
			][_val = px::bind(
				&string_ref::substr, px::construct<string_ref>(px::ref(*m_master_string)),
				px::begin(_1) + 1 - px::ref(m_begin),
				px::size(_1) - 2
			)];
		}

		struct simple_token_checker {
			typedef bool result_type;

			template <typename V>
			bool operator() (const V& parIn) const {
				std::cout << "simple_token_checker returning ";
				if (std::find(std::begin(parIn), std::end(parIn), ' ') == std::end(parIn)) {
					std::cout << "true\n";
					return true;
				}
				else {
					std::cout << "false\n";
					return false;
				}
			}
		};
	} //unnamed namespace

	SplitMime string_to_mime (const std::string* parMime, bool& parParseOk, int& parParsedCharCount) {
		using boost::spirit::qi::blank;
		using boost::spirit::qi::blank_type;

		MimeGrammar<std::string::const_iterator, blank_type> gramm(parMime);
		SplitMime result;

		parParseOk = false;
		parParsedCharCount = 0;

		std::string::const_iterator start_it = parMime->cbegin();
		const bool parse_ok = boost::spirit::qi::phrase_parse(
			start_it,
			parMime->cend(),
			gramm,
			blank,
			result
		);

		parParseOk = parse_ok and (parMime->cend() == start_it);
		parParsedCharCount = std::distance(parMime->cbegin(), start_it);
		assert(parParsedCharCount >= 0);
		return result;
	}

	std::string mime_to_string (const SplitMime& parMime, bool& parWriteOk) {
		namespace px = boost::phoenix;
		using boost::string_ref;
		using boost::spirit::karma::generate;
		using boost::spirit::karma::char_;
		using boost::spirit::karma::string;
		using boost::spirit::karma::rule;
		using boost::spirit::karma::alnum;
		using boost::spirit::karma::eps;
		using boost::spirit::_val;

		if (parMime.type.empty() or parMime.subtype.empty()) {
			parWriteOk = false;
			return std::string();
		}

		px::function<simple_token_checker> is_simple_token;
		std::string retval;
		std::back_insert_iterator<std::string> out_iter(retval);
		rule<std::back_insert_iterator<std::string>, string_ref()> token;
		rule<std::back_insert_iterator<std::string>, string_ref()> quoted_string;
		rule<std::back_insert_iterator<std::string>, string_ref()> param_value;

		token %= eps(is_simple_token(_val)) << *(alnum | char_("._-"));
		quoted_string %= '"' << string << '"';
		param_value %= token | quoted_string;

		parWriteOk = generate(
			out_iter,
			string << "/" << string << *(
				"; " << string << "=" << param_value
			),
			parMime
		);
		return retval;
	}
} //namespace tawashi
