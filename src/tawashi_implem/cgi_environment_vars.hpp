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
	SLOW_ENUM(CGIVars, std::size_t,
		AUTH_TYPE = 0,
		CONTENT_LENGTH,
		CONTENT_TYPE,
		DOCUMENT_ROOT, //The root directory of your server
		GATEWAY_INTERFACE,
		HTTP_CLIENT_IP,
		HTTP_COOKIE, //The visitor's cookie, if one is set
		HTTP_HOST, //The hostname of your server
		HTTP_REFERER, //The URL of the page that called your script
		HTTP_X_FORWARDED_FOR,
		HTTPS, //"on" if the script is being called through a secure server
		HTTP_USER_AGENT, //The browser type of your visitor
		PATH, //The system path your server is running under
		PATH_INFO,
		PATH_TRANSLATED,
		QUERY_STRING, //The query string (see GET, below)
		REMOTE_ADDR, //The IP address of the visitor
		REMOTE_HOST, //The hostname of the visitor (if your server has reverse-name-lookups on; otherwise this is the IP address again)
		REMOTE_IDENT,
		REMOTE_PORT, //The port the visitor is connected to on the web server
		REMOTE_USER, //The visitor's user name (for .htaccess-protected pages)
		REQUEST_METHOD, //GET or POST
		REQUEST_URI, //The interpreted pathname of the requested document or CGI (relative to the document root)
		SCRIPT_FILENAME, //The full pathname of the current CGI
		SCRIPT_NAME, //The interpreted pathname of the current CGI (relative to the document root)
		SERVER_ADMIN, //The email address for your server's webmaster
		SERVER_NAME, //Your server's fully qualified domain name (eg: www.example.com)
		SERVER_PORT, //The port number your server is listening on
		SERVER_PROTOCOL,
		SERVER_SOFTWARE //The server software you're using (such as Apache 1.3)
	);

	std::vector<std::string> cgi_environment_vars (const char* const* parEnvList);
} //namespace tawashi
