#include "cgi_environment_vars.hpp"
#include "get_env.hpp"

namespace tawashi {
	std::vector<std::string> cgi_environment_vars() {
		using boost::string_ref;

		std::vector<std::string> retlist;
		retlist.reserve(CGIVars::_size());

		for (CGIVars var : CGIVars::_values()) {
			auto value = get_env_as<string_ref>(var._to_string(), "");
			retlist.push_back(std::string(value.data(), value.size()));
		}
		return retlist;
	}
} //namespace tawashi
