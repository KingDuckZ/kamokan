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

#pragma once

#include <vector>
#include <string>
#include "enum.h"

namespace tawashi {
	BETTER_ENUM(CGIVars, std::size_t,
		AUTH_TYPE = 0,
		HTTP_USER_AGENT,
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

	std::vector<std::string> cgi_environment_vars();
} //namespace tawashi
