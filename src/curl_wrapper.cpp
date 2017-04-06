#include "curl_wrapper.hpp"
#include <curl/curl.h>
#include <cassert>
#include <ciso646>

#if !defined(NDEBUG)
#	define CURL_WRAPPER_VERBOSE
#endif

#if defined(CURL_WRAPPER_VERBOSE)
#	include <iostream>
#endif

namespace tawashi {
	namespace {
		struct CurlDeleter {
			void operator() (CURL* parCurl) const {
#if defined(CURL_WRAPPER_VERBOSE)
				std::cout << "Deleting CURL* " << parCurl << " and cleaning up\n";
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
			std::cout << "CURL object requested\n";
#endif
			auto shared = curl.lock();
			if (not shared) {
#if defined(CURL_WRAPPER_VERBOSE)
				std::cout << "CURL weak pointer has expired! Calling curl_global_init()\n";
#endif
				if (curl_global_init(CURL_GLOBAL_ALL))
					return CurlWrapper::CurlPtr();
#if defined(CURL_WRAPPER_VERBOSE)
				std::cout << "Calling curl_easy_init()\n";
#endif
				shared.reset(curl_easy_init(), CurlDeleter());

				if (not shared) {
					curl_global_cleanup();
					return CurlWrapper::CurlPtr();
				}
				curl = shared;
			}
#if defined(CURL_WRAPPER_VERBOSE)
			std::cout << "CURL shared pointer ready: " << shared.get() << '\n';
#endif
			assert(shared);
			return shared;
		}
	} //unnamed namespace

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

