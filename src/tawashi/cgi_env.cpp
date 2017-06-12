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
#include "tawashi_exception.hpp"
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
#include <spdlog/spdlog.h>

BOOST_FUSION_ADAPT_STRUCT(
	tawashi::cgi::Env::VersionInfo,
	(boost::string_view, name)
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
			using boost::string_view;
			using VerNum = boost::spirit::qi::uint_parser<uint16_t, 10, 1, 1>;
			using RuleType = boost::spirit::qi::rule<std::string::const_iterator, string_view(), boost::spirit::ascii::space_type>;
			using boost::spirit::_1;
			using boost::spirit::qi::_val;
			using boost::phoenix::begin;
			using boost::phoenix::size;
			using boost::phoenix::construct;
			using boost::make_optional;
			using boost::optional;

			assert(not parString.empty());

			auto beg = parString.cbegin();
			RuleType protocol = raw[+(char_ - '/')][_val = px::bind(&string_view::substr, construct<string_view>(px::ref(parString)), begin(_1) - px::ref(beg), size(_1))];
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

		std::size_t calculate_skip_path_length (const boost::string_view& parPath, const boost::string_view& parBasePath) {
			const std::size_t base_path_tr_slash = (not parBasePath.empty() and parBasePath[parBasePath.size() - 1] == '/' ? 1 : 0);
			boost::string_view base_path = parBasePath.substr(0, parBasePath.size() - base_path_tr_slash);
			SPDLOG_TRACE(spdlog::get("statuslog"), "calculating skip prefix for REQUEST_URI=\"{}\", base path=\"{}\", parBasePath=\"{}\", base path trailing slash={}",
				std::string(parPath.begin(), parPath.end()),
				std::string(base_path.begin(), base_path.end()),
				std::string(parBasePath.begin(), parBasePath.end()),
				base_path_tr_slash
			);

			if (boost::starts_with(parPath, base_path)) {
				//account for the trailing slash in either base path and path info
				return std::min(parBasePath.size() + (1 - base_path_tr_slash), parPath.size());
			}
			else {
				std::string str_base_path(parBasePath.begin(), parBasePath.end());
				std::string str_path(parPath.begin(), parPath.end());
				spdlog::get("statuslog")->error(
					"base path is not a prefix of REQUEST_URI: base path={}, REQUEST_URI={}, tawashi will most likely malfunction",
					str_base_path,
					str_path
				);
				return 1; //try with the default, maybe the user is lucky and it will work (ie base path = /)
			}
		}
	} //unnamed namespace

	boost::string_view drop_arguments (boost::string_view parURI) {
		auto it_found = std::find(parURI.begin(), parURI.end(), '?');
		assert(it_found - parURI.begin() <= parURI.size());
		return parURI.substr(0, it_found - parURI.begin());
	}

	Env::Env(const char* const* parEnvList, const boost::string_view& parBasePath) :
		m_cgi_env(cgi_environment_vars(parEnvList)),
		m_skip_path_info(calculate_skip_path_length(m_cgi_env[CGIVars::REQUEST_URI], parBasePath)),
		m_request_method_type(RequestMethodType::_from_string(m_cgi_env[CGIVars::REQUEST_METHOD].data()))
	{
		{
			const std::string& content_type = m_cgi_env.at(CGIVars::CONTENT_TYPE);
			int parsed_chars;
			bool parse_ok;
			m_split_mime = string_to_mime(&content_type, parse_ok, parsed_chars);
			if (not parse_ok) {
				std::string err_msg = "Parsing failed at position " +
					std::to_string(parsed_chars) + " for input \"" +
					content_type + "\"";
				throw Exception(ErrorReasons::InvalidContentType, boost::string_view(err_msg));
			}
		}
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

	const std::string& Env::path_info() const {
		return m_cgi_env[CGIVars::PATH_INFO];
	}

	const std::string& Env::path_translated() const {
		return m_cgi_env[CGIVars::PATH_TRANSLATED];
	}

	const std::string& Env::query_string() const {
		return m_cgi_env[CGIVars::QUERY_STRING];
	}

	const std::string& Env::http_client_ip() const {
		return m_cgi_env[CGIVars::HTTP_CLIENT_IP];
	}

	const std::string& Env::http_x_forwarded_for() const {
		return m_cgi_env[CGIVars::HTTP_X_FORWARDED_FOR];
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

	RequestMethodType Env::request_method() const {
		return m_request_method_type;
	}

	const std::string& Env::request_uri() const {
		return m_cgi_env[CGIVars::REQUEST_URI];
	}

	const std::string& Env::script_name() const {
		return m_cgi_env[CGIVars::SCRIPT_NAME];
	}

	const std::string& Env::server_name() const {
		return m_cgi_env[CGIVars::SERVER_NAME];
	}

	bool Env::https() const {
		const std::string& val = m_cgi_env[CGIVars::HTTPS];
		return val.size() == 2 and (
			val == "on" or val == "ON" or val == "oN" or val == "On"
		);
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

	const SplitMime& Env::content_type_split() const {
		return m_split_mime;
	}

	std::ostream& Env::print_all (std::ostream& parStream, const char* parNewline) const {
		for (std::size_t z = 0; z < m_cgi_env.size(); ++z) {
			parStream << CGIVars::_from_integral(z) <<
				" = \"" << m_cgi_env[z] << '"' << parNewline;
		}
		return parStream;
	}

	boost::string_view Env::request_uri_relative() const {
		const std::string& path = m_cgi_env[CGIVars::REQUEST_URI];
		assert(m_skip_path_info <= path.size());
		return boost::string_view(path).substr(m_skip_path_info);
	}

	boost::string_view Env::path_info_relative() const {
		const std::string& path = m_cgi_env[CGIVars::PATH_INFO];
		assert(m_skip_path_info <= path.size());
		return boost::string_view(path).substr(m_skip_path_info);
	}
} //namespace cgi
} //namespace tawashi
