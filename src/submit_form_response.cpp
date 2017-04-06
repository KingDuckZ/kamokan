#include "submit_form_response.hpp"
#include "incredis/incredis.hpp"
#include "cgi_post.hpp"
#include "num_to_token.hpp"
#include <ciso646>

namespace tawashi {
	namespace {
		const char g_post_key[] = "pastie";
	} //unnamed namespace

	SubmitFormResponse::SubmitFormResponse (redis::IncRedis& parRedis) :
		Response("text/plain"),
		m_redis(parRedis)
	{
	}

	void SubmitFormResponse::on_send (std::ostream& parStream) {
		auto post = cgi::read_post(cgi_env());
		auto post_data_it = post.find(g_post_key);
		if (post.end() == post_data_it) {
			parStream << "can't find POST data\n";
		}
		else if (submit_to_redis(post_data_it->second)) {
			parStream << "post submitted correctly\n";
		}
		else {
			parStream << "something happened? :/\n";
		}
	}

	bool SubmitFormResponse::submit_to_redis (const std::string& parText) const {
		if (not m_redis.is_connected()) {
			m_redis.connect();
			m_redis.wait_for_connect();
			if (not m_redis.is_connected())
				return false;
		}

		const auto next_id = m_redis.incr("paste_counter");
		const std::string token = num_to_token(next_id);
		assert(not token.empty());
		return m_redis.set(token, parText);
	}
} //namespace tawashi
