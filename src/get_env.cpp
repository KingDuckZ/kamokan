#include "get_env.hpp"
#include "duckhandy/lexical_cast.hpp"
#include <cstdlib>

namespace tawashi {
	boost::optional<boost::string_ref> get_env (const char* parName) {
		using boost::string_ref;
		using boost::make_optional;
		using boost::optional;

		const char* const raw_getvar = secure_getenv(parName);
		return (raw_getvar ? make_optional(string_ref(raw_getvar)) : optional<string_ref>());
	}

	template <>
	std::string get_env_as (const char* parName, const std::string& parDefault) {
		auto var = get_env(parName);
		return (var ? std::string(var->data(), var->size()) : parDefault);
	}

	template <>
	boost::string_ref get_env_as (const char* parName, const boost::string_ref& parDefault) {
		auto var = get_env(parName);
		return (var ? *var : parDefault);
	}

	template <>
	std::size_t get_env_as (const char* parName, const std::size_t& parDefault) {
		using dhandy::lexical_cast;
		auto var = get_env(parName);
		return (var ? lexical_cast<std::size_t>(*var) : parDefault);
	}
} //namespace tawashi
