#pragma once

#include <vector>
#include <string>
#include "enum.h"

namespace tawashi {
	BETTER_ENUM(CGIVars, std::size_t,
		AUTH_TYPE = 0,
		CONTENT_LENGTH,
		CONTENT_TYPE,
		GATEWAY_INTERFACE,
		PATH_INFO,
		PATH_TRANSLATED,
		QUERY_STRING,
		REMOTE_ADDR,
		REMOTE_HOST,
		REMOTE_IDENT,
		REMOTE_USER,
		REQUEST_METHOD,
		SCRIPT_NAME,
		SERVER_NAME,
		SERVER_PORT,
		SERVER_PROTOCOL,
		SERVER_SOFTWARE
	);

	std::vector<std::string> get_cgi_environment_vars();
} //namespace tawashi
