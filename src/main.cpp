#include "incredis/incredis.hpp"
#include "submit_form_response.hpp"
#include "cgi_env.hpp"
#include <iostream>
#include <string>
#include <algorithm>
#include <iterator>

//www.open-std.org/jtc1/sc22/wg21/docs/papers/2014/n4150.pdf

namespace {
} //unnamed namespace

int main() {
	//std::cout << "Content-type:text/plain\n\n";

	redis::IncRedis incredis("127.0.0.1", 6379);

	tawashi::SubmitFormResponse resp;
	resp.send();

	tawashi::CGIEnv cgi_env;
	for (auto& pair : cgi_env.query_string()) {
		std::cout << "first:\t\"" << pair.first <<
			"\"\tsecond:\t\"" << pair.second << "\"\n";
	}

	const std::size_t in_len = cgi_env.content_length();
	std::cout << "\n<br>\n";
	std::cout << "Content length: \"" << in_len << "\"\n<br>\n";

	cgi_env.print_all(std::cout, "<br>\n");
	std::string input;
	if (in_len > 0)
		std::copy_n(std::istream_iterator<char>(std::cin), in_len, std::back_inserter(input));
	std::cout << input << '\n';

	auto ver = cgi_env.gateway_interface();
	if (ver)
		std::cout << ver->name << " - v" << ver->major << ',' << ver->minor << "<br>\n";

	return 0;
}
