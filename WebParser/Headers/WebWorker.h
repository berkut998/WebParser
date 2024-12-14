#pragma once
#include "MemoryStruct.h"
#include <iostream>
#include <stdio.h>
#include <curl/curl.h>
#include <string>
#include <map>
#include <boost/beast/core/detail/base64.hpp>
#include <vector>
//#include "ErrorHandler.h"

namespace {
	class WebWorker final
	{
	public:
		WebWorker();
		WebWorker(std::string userAgent);
		WebWorker(std::string userAgent, bool setCookies);
		~WebWorker();
		MemoryStruct getWebPage(const char* url);
		MemoryStruct postToWebPage(const char* url, std::map<std::string, std::string> files);
		MemoryStruct postToWebPage(const char* url, std::string fields);
		bool download_file(const char* url, const char* filename);
		void auth(std::string login, std::string password, std::string url, std::string url_auth = "");
		static std::string removeHtmlTags(std::string text);
		std::string urlEscape(std::string text);
		std::string urlEscapeWhiteSpaces(std::string text);
		bool ftpLoad(std::string pathToImage, std::string pathToLoad);
	private:
		static size_t WriteMemoryCallback(void* contents, size_t size, size_t nmemb, void* userp);
		static size_t read_callback(void* ptr, size_t size, size_t nmemb, void* userdata);
		size_t callbackfunction(void* ptr, size_t size, size_t nmemb, void* userdata);

		CURL* curl;
		CURLcode curlCode;
		std::string userAgent;
		struct MemoryStruct chunk;

	};
}


