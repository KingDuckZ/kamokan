#include "incredis/incredis.hpp"
#include "submit_form_response.hpp"
#include "index_response.hpp"
#include "cgi_env.hpp"
#include <iostream>
#include <string>

//www.open-std.org/jtc1/sc22/wg21/docs/papers/2014/n4150.pdf

namespace {
} //unnamed namespace

int main() {
	//std::cout << "Content-type:text/plain\n\n";

	redis::IncRedis incredis("127.0.0.1", 6379);
	incredis.connect();

	tawashi::CGIEnv cgi_env;
	if (cgi_env.path_info() == "/index.cgi") {
		tawashi::IndexResponse resp;
		resp.send();
	}
	else if (cgi_env.path_info() == "/paste.cgi") {
		tawashi::SubmitFormResponse resp(incredis);
		resp.send();
	}
	else {
		std::cout << "Content-type:text/plain\n\n";
		std::cout << "you shouldn't be here\n";
	}

	return 0;
}
