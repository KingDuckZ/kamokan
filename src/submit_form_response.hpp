#pragma once

#include "response.hpp"
#include <string>

namespace redis {
	class IncRedis;
} //namespace redis

namespace tawashi {
	class SubmitFormResponse : public Response {
	public:
		explicit SubmitFormResponse (redis::IncRedis& parRedis);

	private:
		virtual void on_send (std::ostream& parStream) override;
		bool submit_to_redis (const std::string& parText) const;

		redis::IncRedis& m_redis;
	};
} //namespace tawashi
