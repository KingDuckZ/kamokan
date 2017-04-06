#pragma once

#include "response.hpp"

namespace redis {
	class IncRedis;
} //namespace redis

namespace tawashi {
	class SubmitFormResponse : public Response {
	public:
		explicit SubmitFormResponse (redis::IncRedis& parRedis);

	private:
		virtual void on_send (std::ostream& parStream) override;
		redis::IncRedis& m_redis;
	};
} //namespace tawashi
