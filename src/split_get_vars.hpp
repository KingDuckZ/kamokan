#pragma once

#include "duckhandy/compatibility.h"
#include <boost/utility/string_ref.hpp>
#include <vector>
#include <utility>

namespace tawashi {
	std::vector<std::pair<boost::string_ref, boost::string_ref>> split_env_vars ( const std::string& parCommaSeparatedList ) a_pure;
} //namespace tawashi
