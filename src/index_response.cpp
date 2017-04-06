#include "index_response.hpp"

namespace tawashi {
	IndexResponse::IndexResponse() :
		Response("text/html")
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

