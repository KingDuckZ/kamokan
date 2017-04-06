#pragma once

#include "cgi_env.hpp"
#include <string>
#include <iostream>

namespace tawashi {
	class Response {
	public:
		virtual ~Response() noexcept;

		void send();

	protected:
		Response (std::string&& parType);
		const CGIEnv& cgi_env() const;

	private:
		virtual void on_send (std::ostream& parStream) = 0;

		CGIEnv m_cgi_env;
		std::string m_content_type;
	};
} //namespace tawashi
