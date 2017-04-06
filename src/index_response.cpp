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

#include "index_response.hpp"

namespace tawashi {
	IndexResponse::IndexResponse() :
		Response(Response::ContentType, "text/html")
	{
	}

	void IndexResponse::on_send (std::ostream& parStream) {
		parStream <<
			R"(
<form action="http://127.0.0.1:8080/paste.cgi" method="POST" accept-charset="UTF-8">
	<textarea name="pastie" cols="80" rows="24"></textarea>
	<br>
		<button type="submit">tawashi</button>
	</br>
</form>
)";
	}
} //namespace tawashi

