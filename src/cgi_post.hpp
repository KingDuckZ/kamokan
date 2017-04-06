#pragma once

#include <boost/container/flat_map.hpp>
#include <boost/utility/string_ref.hpp>

namespace tawashi {
	class CGIEnv;

	namespace cgi {
		typedef boost::container::flat_map<boost::string_ref, boost::string_ref> PostMapType;

		const PostMapType& read_post (const CGIEnv& parEnv);
	} //namespace cgi
} //namespace tawashi
