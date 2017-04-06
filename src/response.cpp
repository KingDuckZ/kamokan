#include "response.hpp"
#include <utility>

namespace tawashi {
	Response::Response (std::string&& parType) :
		m_content_type(std::move(parType))
	{
	}

	Response::~Response() noexcept = default;

	void Response::send() {
		std::cout << "Content-type:" << m_content_type << "\n\n";
		this->on_send(std::cout);
		std::cout.flush();
	}

	const CGIEnv& Response::cgi_env() const {
		return m_cgi_env;
	}
} //namespace tawashi
