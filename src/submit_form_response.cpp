#include "submit_form_response.hpp"
#include "incredis/incredis.hpp"
#include "cgi_post.hpp"

namespace tawashi {
	namespace {
		const char g_post_key[] = "tawashi";

		bool submit_to_redis (const std::string& parText) {
			return true;
		}
	} //unnamed namespace

	SubmitFormResponse::SubmitFormResponse (redis::IncRedis& parRedis) :
		Response("text/html"),
		m_redis(parRedis)
	{
	}

	void SubmitFormResponse::on_send (std::ostream& parStream) {
		auto post = cgi::read_post(cgi_env());
		auto post_data_it = post.find(g_post_key);
		if (post.end() != post_data_it) {
			parStream << "can't find POST data\n";
		}
		else if (submit_to_redis(post_data_it->second)) {
			parStream << "post submitted correctly\n";
		}
	}
} //namespace tawashi
