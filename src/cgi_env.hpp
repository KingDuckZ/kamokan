#pragma once

#include "split_get_vars.hpp"
#include <vector>
#include <string>
#include <boost/utility/string_ref.hpp>
#include <cstdint>

namespace tawashi {
	struct VersionInfo {
		boost::string_ref name;
		uint16_t major;
		uint16_t minor;
	};

	class CGIEnv {
	public:
		CGIEnv();
		~CGIEnv() noexcept;

		const std::string& auth_type() const;
		std::size_t content_length() const;
		const std::string& content_type() const;
		VersionInfo gateway_interface() const;
		const std::string& path_info() const;
		const std::string& path_translated() const;
		KeyValueList query_string() const;
		const std::string& remote_addr() const;
		const std::string& remote_host() const;
		const std::string& remote_ident() const;
		const std::string& remote_user() const;
		const std::string& request_method() const;
		const std::string& script_name() const;
		const std::string& server_name() const;
		uint16_t server_port() const;
		VersionInfo server_protocol() const;
		const std::string& server_software() const;

	private:
		std::vector<std::string> m_cgi_env;
	};
} //namespace tawashi
