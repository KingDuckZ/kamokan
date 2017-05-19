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

#include "cgi_env.hpp"
#include "cgi_environment_vars.hpp"
#include "duckhandy/lexical_cast.hpp"
#include <cassert>
#include <ciso646>
#include <boost/spirit/include/qi_core.hpp>
#include <boost/spirit/include/qi_numeric.hpp>
#include <boost/spirit/include/qi_plus.hpp>
#include <boost/spirit/include/qi_raw.hpp>
#include <boost/spirit/include/qi_lit.hpp>
#include <boost/phoenix/object/construct.hpp>
#include <boost/phoenix/bind/bind_member_function.hpp>
#include <boost/phoenix/operator.hpp>
#include <boost/fusion/adapted/struct.hpp>
#include <boost/phoenix/stl/container.hpp>
#include <boost/algorithm/string/predicate.hpp>

BOOST_FUSION_ADAPT_STRUCT(
	tawashi::cgi::Env::VersionInfo,
	(boost::string_ref, name)
	(uint16_t, major)
	(uint16_t, minor)
);

namespace tawashi {
namespace cgi {
	namespace {
		boost::optional<Env::VersionInfo> split_version (const std::string& parString) {
			namespace px = boost::phoenix;

			using boost::spirit::ascii::space;
			using boost::spirit::qi::raw;
			using boost::spirit::qi::char_;
			using boost::string_ref;
			using VerNum = boost::spirit::qi::uint_parser<uint16_t, 10, 1, 1>;
			using RuleType = boost::spirit::qi::rule<std::string::const_iterator, string_ref(), boost::spirit::ascii::space_type>;
			using boost::spirit::_1;
			using boost::spirit::qi::_val;
			using boost::phoenix::begin;
			using boost::phoenix::size;
			using boost::phoenix::construct;
			using boost::make_optional;
			using boost::optional;

			assert(not parString.empty());

			auto beg = parString.cbegin();
			RuleType protocol = raw[+(char_ - '/')][_val = px::bind(&string_ref::substr, construct<string_ref>(px::ref(parString)), begin(_1) - px::ref(beg), size(_1))];
			VerNum ver_num;

			auto it_curr = parString.cbegin();
			Env::VersionInfo retval;
			const bool parse_ret = boost::spirit::qi::phrase_parse(
				it_curr,
				parString.end(),
				protocol >> '/' >> ver_num >> '.' >> ver_num,
				space,
				retval
			);

			if (parse_ret and parString.end() == it_curr)
				return make_optional(retval);
			else
				return optional<Env::VersionInfo>();
		}
	} //unnamed namespace

	Env::Env(const char* const* parEnvList, const boost::string_ref& parBasePath) :
		m_cgi_env(cgi_environment_vars(parEnvList)),
		m_skip_path_info(0)
	{
		const std::string& path = m_cgi_env[CGIVars::PATH_INFO];
		assert(parBasePath.size() <= path.size());
		if (boost::starts_with(path, parBasePath))
			m_skip_path_info = parBasePath.size();
		else
			m_skip_path_info = 0;
	}

	Env::~Env() noexcept = default;

	const std::string& Env::auth_type() const {
		return m_cgi_env[CGIVars::AUTH_TYPE];
	}

	std::size_t Env::content_length() const {
		using dhandy::lexical_cast;
		const std::string& value = m_cgi_env[CGIVars::CONTENT_LENGTH];
		return (value.empty() ? 0U : lexical_cast<std::size_t>(value));
	}

	const std::string& Env::content_type() const {
		return m_cgi_env[CGIVars::CONTENT_TYPE];
	}

	auto Env::gateway_interface() const -> boost::optional<VersionInfo> {
		return split_version(m_cgi_env[CGIVars::GATEWAY_INTERFACE]);
	}

	boost::string_ref Env::path_info() const {
		const std::string& path = m_cgi_env[CGIVars::PATH_INFO];
		assert(m_skip_path_info <= path.size());
		return boost::string_ref(path).substr(m_skip_path_info);
	}

	const std::string& Env::path_translated() const {
		return m_cgi_env[CGIVars::PATH_TRANSLATED];
	}

	const std::string& Env::query_string() const {
		return m_cgi_env[CGIVars::QUERY_STRING];
	}

	const std::string& Env::remote_addr() const {
		return m_cgi_env[CGIVars::REMOTE_ADDR];
	}

	const std::string& Env::remote_host() const {
		return m_cgi_env[CGIVars::REMOTE_HOST];
	}

	const std::string& Env::remote_ident() const {
		return m_cgi_env[CGIVars::REMOTE_IDENT];
	}

	const std::string& Env::remote_user() const {
		return m_cgi_env[CGIVars::REMOTE_USER];
	}

	const std::string& Env::request_method() const {
		return m_cgi_env[CGIVars::REQUEST_METHOD];
	}

	const std::string& Env::script_name() const {
		return m_cgi_env[CGIVars::SCRIPT_NAME];
	}

	const std::string& Env::server_name() const {
		return m_cgi_env[CGIVars::SERVER_NAME];
	}

	bool Env::https() const {
		return m_cgi_env[CGIVars::HTTPS] == "on";
	}

	uint16_t Env::server_port() const {
		using dhandy::lexical_cast;
		const std::string& value = m_cgi_env[CGIVars::SERVER_PORT];
		return (value.empty() ? 0U : lexical_cast<uint16_t>(value));
	}

	auto Env::server_protocol() const -> boost::optional<VersionInfo> {
		return split_version(m_cgi_env[CGIVars::SERVER_PROTOCOL]);
	}

	const std::string& Env::server_software() const {
		return m_cgi_env[CGIVars::SERVER_SOFTWARE];
	}

	Env::GetMapType Env::query_string_split() const {
		GetMapType retval;
		const auto urlencoded_values = split_env_vars(m_cgi_env[CGIVars::QUERY_STRING]);
		retval.reserve(urlencoded_values.size());
		for (auto& itm : urlencoded_values) {
			retval[m_houdini.unescape_url(itm.first)] = m_houdini.unescape_url(itm.second);
		}
		return retval;
	}

	std::ostream& Env::print_all (std::ostream& parStream, const char* parNewline) const {
		for (std::size_t z = 0; z < m_cgi_env.size(); ++z) {
			parStream << CGIVars::_from_integral(z) <<
				" = \"" << m_cgi_env[z] << '"' << parNewline;
		}
		return parStream;
	}

} //namespace cgi
} //namespace tawashi
