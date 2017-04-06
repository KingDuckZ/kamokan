#pragma once

#include "response.hpp"

namespace tawashi {
	class IndexResponse : public Response {
	public:
		IndexResponse();

	private:
		virtual void on_send (std::ostream& parStream) override;
	};
} //namespace tawashi
