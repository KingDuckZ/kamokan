#include "incredis/incredis.hpp"
#include "split_get_vars.hpp"
#include "submit_form_response.hpp"
#include "envy.hpp"
#include "duckhandy/lexical_cast.hpp"
#include <iostream>
#include <string>
#include <algorithm>
#include <iterator>

//www.open-std.org/jtc1/sc22/wg21/docs/papers/2014/n4150.pdf

namespace {
} //unnamed namespace

int main() {
	using boost::string_ref;
	using dhandy::lexical_cast;

	std::vector<std::string> env = tawashi::get_cgi_vars();
	//std::cout << "Content-type:text/plain\n\n";
	const std::string& getvar = env[tawashi::CGIVars::QUERY_STRING];
	//std::cout << "QUERY_STRING = \"" << getvar << "\"\n";

	redis::IncRedis incredis("127.0.0.1", 6379);

	tawashi::SubmitFormResponse resp;
	resp.send();

	for (auto& pair : tawashi::split_env_vars(getvar)) {
		std::cout << "first:\t\"" << pair.first <<
			"\"\tsecond:\t\"" << pair.second << "\"\n";
	}

	const std::size_t in_len = lexical_cast<std::size_t>(env[tawashi::CGIVars::CONTENT_LENGTH]);
	std::cout << "\n<br>\n";
	std::cout << "Content length: \"" << in_len << "\"\n<br>\n";

	for (std::size_t z = 0; z < env.size(); ++z) {
		std::cout << tawashi::CGIVars::_from_integral(z) << " = \"" << env[z] << "\"<br>\n";
	}
	std::string input;
	if (in_len > 0)
		std::copy_n(std::istream_iterator<char>(std::cin), in_len, std::back_inserter(input));
	std::cout << input << '\n';

	return 0;
}
