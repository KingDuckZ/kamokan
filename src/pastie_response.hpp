#pragma once

#include "response.hpp"
#include <string>

namespace redis {
	class IncRedis;
} //namespace redis

namespace tawashi {
	class PastieResponse : public Response {
	public:
		PastieResponse (redis::IncRedis& parRedis);

	private:
		virtual void on_send (std::ostream& parStream) override;

		redis::IncRedis& m_redis;
	};
} //namespace tawashi
