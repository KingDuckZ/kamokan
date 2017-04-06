#pragma once

#include <boost/container/flat_map.hpp>
#include <string>

namespace tawashi {

	namespace cgi {
		class Env;

		typedef boost::container::flat_map<std::string, std::string> PostMapType;

		const PostMapType& read_post (const Env& parEnv);
	} //namespace cgi
} //namespace tawashi
