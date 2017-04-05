#pragma once

#include "split_get_vars.hpp"
#include "duckhandy/compatibility.h"
#include <vector>
#include <string>
#include <boost/utility/string_ref.hpp>
#include <cstdint>
#include <iostream>
#include <boost/optional.hpp>

namespace tawashi {
	class CGIEnv {
	public:
		struct VersionInfo {
			boost::string_ref name;
			uint16_t major;
			uint16_t minor;
		};

		CGIEnv();
		~CGIEnv() noexcept;

		const std::string& auth_type() const;
		std::size_t content_length() const;
		const std::string& content_type() const;
		boost::optional<VersionInfo> gateway_interface() const a_pure;
		const std::string& path_info() const;
		const std::string& path_translated() const;
		const std::string& query_string() const;
		const std::string& remote_addr() const;
		const std::string& remote_host() const;
		const std::string& remote_ident() const;
		const std::string& remote_user() const;
		const std::string& request_method() const;
		const std::string& script_name() const;
		const std::string& server_name() const;
		uint16_t server_port() const a_pure;
		boost::optional<VersionInfo> server_protocol() const a_pure;
		const std::string& server_software() const;

		KeyValueList query_string_split() const a_pure;

		std::ostream& print_all (std::ostream& parStream, const char* parNewline) const;

	private:
		std::vector<std::string> m_cgi_env;
	};
} //namespace tawashi
