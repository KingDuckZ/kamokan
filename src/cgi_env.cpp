#include "cgi_env.hpp"
#include "envy.hpp"
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

BOOST_FUSION_ADAPT_STRUCT(
	tawashi::CGIEnv::VersionInfo,
	(boost::string_ref, name)
	(uint16_t, major)
	(uint16_t, minor)
);

namespace tawashi {
	namespace {
		boost::optional<CGIEnv::VersionInfo> split_version (const std::string& parString) {
			namespace px = boost::phoenix;

			using boost::spirit::ascii::space;
			using boost::spirit::qi::raw;
			using boost::spirit::qi::char_;
			using boost::spirit::qi::int_;
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
			CGIEnv::VersionInfo retval;
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
				return optional<CGIEnv::VersionInfo>();
		}
	} //unnamed namespace

	CGIEnv::CGIEnv() :
		m_cgi_env(get_cgi_environment_vars())
	{
	}

	CGIEnv::~CGIEnv() noexcept = default;

	const std::string& CGIEnv::auth_type() const {
		return m_cgi_env[CGIVars::AUTH_TYPE];
	}

	std::size_t CGIEnv::content_length() const {
		using dhandy::lexical_cast;
		const std::string& value = m_cgi_env[CGIVars::CONTENT_LENGTH];
		return (value.empty() ? 0U : lexical_cast<std::size_t>(value));
	}

	const std::string& CGIEnv::content_type() const {
		return m_cgi_env[CGIVars::CONTENT_TYPE];
	}

	auto CGIEnv::gateway_interface() const -> boost::optional<VersionInfo> {
		return split_version(m_cgi_env[CGIVars::GATEWAY_INTERFACE]);
	}

	const std::string& CGIEnv::path_info() const {
		return m_cgi_env[CGIVars::PATH_INFO];
	}

	const std::string& CGIEnv::path_translated() const {
		return m_cgi_env[CGIVars::PATH_TRANSLATED];
	}

	const std::string& CGIEnv::query_string() const {
		return m_cgi_env[CGIVars::QUERY_STRING];
	}

	const std::string& CGIEnv::remote_addr() const {
		return m_cgi_env[CGIVars::REMOTE_ADDR];
	}

	const std::string& CGIEnv::remote_host() const {
		return m_cgi_env[CGIVars::REMOTE_HOST];
	}

	const std::string& CGIEnv::remote_ident() const {
		return m_cgi_env[CGIVars::REMOTE_IDENT];
	}

	const std::string& CGIEnv::remote_user() const {
		return m_cgi_env[CGIVars::REMOTE_USER];
	}

	const std::string& CGIEnv::request_method() const {
		return m_cgi_env[CGIVars::REQUEST_METHOD];
	}

	const std::string& CGIEnv::script_name() const {
		return m_cgi_env[CGIVars::SCRIPT_NAME];
	}

	const std::string& CGIEnv::server_name() const {
		return m_cgi_env[CGIVars::SERVER_NAME];
	}

	uint16_t CGIEnv::server_port() const {
		using dhandy::lexical_cast;
		const std::string& value = m_cgi_env[CGIVars::SERVER_PORT];
		return (value.empty() ? 0U : lexical_cast<uint16_t>(value));
	}

	auto CGIEnv::server_protocol() const -> boost::optional<VersionInfo> {
		return split_version(m_cgi_env[CGIVars::SERVER_PROTOCOL]);
	}

	const std::string& CGIEnv::server_software() const {
		return m_cgi_env[CGIVars::SERVER_SOFTWARE];
	}

	KeyValueList CGIEnv::query_string_split() const {
		return split_env_vars(m_cgi_env[CGIVars::QUERY_STRING]);
	}

	std::ostream& CGIEnv::print_all (std::ostream& parStream, const char* parNewline) const {
		for (std::size_t z = 0; z < m_cgi_env.size(); ++z) {
			parStream << CGIVars::_from_integral(z) <<
				" = \"" << m_cgi_env[z] << '"' << parNewline;
		}
		return parStream;
	}
} //namespace tawashi
