#pragma once

#include <boost/container/flat_map.hpp>
#include <string>

namespace tawashi {
	class CGIEnv;

	namespace cgi {
		typedef boost::container::flat_map<std::string, std::string> PostMapType;

		const PostMapType& read_post (const CGIEnv& parEnv);
	} //namespace cgi
} //namespace tawashi
