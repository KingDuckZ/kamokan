/* Copyright 2017, Michele Santullo
 * This file is part of "tawashi".
 *
 * "tawashi" is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * "tawashi" is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with "tawashi".  If not, see <http://www.gnu.org/licenses/>.
 */

#include "curl_wrapper.hpp"
#include <curl/curl.h>
#include <cassert>
#include <ciso646>
#include <algorithm>

#if !defined(NDEBUG)
//#	define CURL_WRAPPER_VERBOSE
#endif

#if defined(CURL_WRAPPER_VERBOSE)
#	include <iostream>
#endif

namespace tawashi {
	namespace {
		struct CurlDeleter {
			void operator() (CURL* parCurl) const {
#if defined(CURL_WRAPPER_VERBOSE)
				std::cerr << "Deleting CURL* " << parCurl << " and cleaning up\n";
#endif
				assert(parCurl);
				curl_easy_cleanup(parCurl);
				curl_global_cleanup();
			}
		};

		struct CurlBufferDeleter {
			void operator() (char* parPtr) {
				assert(parPtr);
				curl_free(parPtr);
			}
		};

		typedef std::unique_ptr<char, CurlBufferDeleter> CurlBufferPointer;

		CurlWrapper::CurlPtr get_new_curl() {
			static std::weak_ptr<CURL> curl;

#if defined(CURL_WRAPPER_VERBOSE)
			std::cerr << "CURL object requested\n";
#endif
			auto shared = curl.lock();
			if (not shared) {
#if defined(CURL_WRAPPER_VERBOSE)
				std::cerr << "CURL weak pointer has expired! Calling curl_global_init()\n";
#endif
				if (curl_global_init(CURL_GLOBAL_ALL))
					return CurlWrapper::CurlPtr();
#if defined(CURL_WRAPPER_VERBOSE)
				std::cerr << "Calling curl_easy_init()\n";
#endif
				shared.reset(curl_easy_init(), CurlDeleter());

				if (not shared) {
					curl_global_cleanup();
					return CurlWrapper::CurlPtr();
				}
				curl = shared;
			}
#if defined(CURL_WRAPPER_VERBOSE)
			std::cerr << "CURL shared pointer ready: " << shared.get() << '\n';
#endif
			assert(shared);
			return shared;
		}
	} //unnamed namespace

	std::string unescape_string (const CurlWrapper& parCurl, const boost::string_ref& parString) {
		if (parString.empty())
			return std::string();

		std::string new_value(parString.data(), parString.size());
		std::replace(new_value.begin(), new_value.end(), '+', ' ');
		return parCurl.url_unescape(new_value);
	}

	CurlWrapper::CurlWrapper() :
		m_curl(get_new_curl())
	{
		assert(m_curl);
	}

	CurlWrapper::~CurlWrapper() noexcept = default;

	std::string CurlWrapper::url_escape (const boost::string_ref& parText) const {
		const CurlBufferPointer buff(curl_easy_escape(m_curl.get(), parText.data(), parText.size()));
		return std::string(buff.get());
	}

	std::string CurlWrapper::url_unescape (const boost::string_ref& parText) const {
		int outLen;
		const CurlBufferPointer buff(curl_easy_unescape(m_curl.get(), parText.data(), parText.size(), &outLen));
		return std::string(buff.get(), outLen);
	}
} //namespace tawashi

