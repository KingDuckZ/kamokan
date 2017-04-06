#include "pastie_response.hpp"
#include "incredis/incredis.hpp"
#include <ciso646>

namespace tawashi {
	PastieResponse::PastieResponse (redis::IncRedis& parRedis) :
		Response("text/plain"),
		m_redis(parRedis)
	{
	}

	void PastieResponse::on_send (std::ostream& parStream) {
		using opt_string = redis::IncRedis::opt_string;

		if (cgi_env().path_info().empty()) {
			return;
		}

		auto token = boost::string_ref(cgi_env().path_info()).substr(1);
		opt_string pastie = m_redis.get(token);
		if (not pastie) {
		}

		parStream << *pastie;
	}
} //namespace tawashi
