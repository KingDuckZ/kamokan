#pragma once

#include <string>
#include <iostream>

namespace tawashi {
	class Response {
	public:
		virtual ~Response() noexcept;

		void send();

	protected:
		Response (std::string&& parType);

	private:
		virtual void on_send (std::ostream& parStream) = 0;

		std::string m_content_type;
	};
} //namespace tawashi
