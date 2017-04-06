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

#include "incredis/incredis.hpp"
#include "submit_paste_response.hpp"
#include "pastie_response.hpp"
#include "index_response.hpp"
#include "cgi_env.hpp"
#include <iostream>
#include <string>

//www.open-std.org/jtc1/sc22/wg21/docs/papers/2014/n4150.pdf

namespace {
} //unnamed namespace

int main() {
	//std::cout << "Content-type:text/plain\n\n";

	redis::IncRedis incredis("127.0.0.1", 6379);
	incredis.connect();

	tawashi::cgi::Env cgi_env;
	if (cgi_env.path_info() == "/index.cgi") {
		tawashi::IndexResponse resp;
		resp.send();
	}
	else if (cgi_env.path_info() == "/paste.cgi") {
		tawashi::SubmitPasteResponse resp(incredis);
		resp.send();
	}
	else {
		tawashi::PastieResponse resp(incredis);
		resp.send();
	}

	return 0;
}
