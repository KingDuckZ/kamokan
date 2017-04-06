#pragma once

#include <boost/utility/string_ref.hpp>
#include <string>
#include <memory>

typedef void CURL;

namespace tawashi {
	class CurlWrapper {
	public:
		typedef std::shared_ptr<CURL> CurlPtr;

		CurlWrapper();
		CurlWrapper (const CurlWrapper&) = delete;
		~CurlWrapper() noexcept;

		std::string url_escape (const boost::string_ref& parText) const;
		std::string url_unescape (const boost::string_ref& parText) const;

	private:
		CurlPtr m_curl;
	};

	std::string unescape_string (const CurlWrapper& parCurl, const boost::string_ref& parString);
} //namespace tawashi
