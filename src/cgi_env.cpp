#include "cgi_env.hpp"
#include "envy.hpp"
#include "duckhandy/lexical_cast.hpp"

namespace tawashi {
	CGIEnv::CGIEnv() :
		m_cgi_env(get_cgi_vars())
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

	VersionInfo CGIEnv::gateway_interface() const {
	}

	const std::string& CGIEnv::path_info() const {
		return m_cgi_env[CGIVars::PATH_INFO];
	}

	const std::string& CGIEnv::path_translated() const {
		return m_cgi_env[CGIVars::PATH_TRANSLATED];
	}

	KeyValueList CGIEnv::query_string() const {
		return split_env_vars(m_cgi_env[CGIVars::QUERY_STRING]);
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

	VersionInfo CGIEnv::server_protocol() const {
	}

	const std::string& CGIEnv::server_software() const {
		return m_cgi_env[CGIVars::SERVER_SOFTWARE];
	}
} //namespace tawashi
