#pragma once

#include "response.hpp"

namespace tawashi {
	class SubmitFormResponse : public Response {
	public:
		SubmitFormResponse();

	private:
		virtual void on_send (std::ostream& parStream) override;
	};
} //namespace tawashi
