#include "submit_form_response.hpp"

namespace tawashi {
	SubmitFormResponse::SubmitFormResponse() :
		Response("text/html")
	{
	}

	void SubmitFormResponse::on_send (std::ostream& parStream) {
		parStream <<
			R"(
<form action="http://127.0.0.1:8080" method="POST" accept-charset="UTF-8">
	<textarea name="tawashi" cols="80" rows="24"></textarea>
	<br>
		<button type="submit">tawashi</button>
	</br>
</form>
)";
	}
} //namespace tawashi
