#pragma once

#include <boost/utility/string_ref.hpp>
#include <string>
#if !defined(_GNU_SOURCE)
#	define _GNU_SOURCE
#endif
#include <cstddef>
#include <utility>
#include <boost/optional.hpp>

namespace tawashi {
	boost::optional<boost::string_ref> get_env (const char* parName);

	template <typename A>
	A get_env_as (const char* parName, const A& parDefault);

	template <>
	std::string get_env_as (const char* parName, const std::string& parDefault);
	template <>
	boost::string_ref get_env_as (const char* parName, const boost::string_ref& parDefault);
	template <>
	std::size_t get_env_as (const char* parName, const std::size_t& parDefault);
} //namespace tawashi
