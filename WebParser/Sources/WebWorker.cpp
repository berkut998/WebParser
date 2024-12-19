#include "WebWorker.h"
namespace WP {

    WebWorker::WebWorker(std::string userAgent, bool setCookies)
    {
        curl = curl_easy_init();
        if (setCookies == true)
        {
            curl_easy_setopt(curl, CURLOPT_COOKIEJAR, "cookies.txt");
            curl_easy_setopt(curl, CURLOPT_COOKIEFILE, "cookies.txt");
        }
        userAgent = userAgent;
    }

    void WebWorker::auth(std::string login, std::string password, std::string url_login, std::string url_auth)
    {
        free(chunk.memory);
        curl_easy_setopt(curl, CURLOPT_POST, 1);
        curl_easy_setopt(curl, CURLOPT_COOKIESESSION, true);
        curl_easy_setopt(curl, CURLOPT_URL, url_login.c_str());
        curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
        curl_easy_setopt(curl, CURLOPT_DEFAULT_PROTOCOL, "https");
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void*)&chunk);
        curl_easy_setopt(curl, CURLOPT_COOKIEJAR, "cookies.txt");
        curl_easy_setopt(curl, CURLOPT_COOKIEFILE, "cookies.txt");
        struct curl_slist* headers = NULL;
        std::string credential = login + ":" + password;
        size_t  encodedTextSize = boost::beast::detail::base64::encoded_size(credential.size());
        char* encoded = new char[encodedTextSize];
        boost::beast::detail::base64::encode(encoded, credential.c_str(), credential.size());
        std::string encodeBase64(encoded);
        size_t paddingStart = encodeBase64.size();
        encodeBase64 = encodeBase64.erase(encodedTextSize, paddingStart - encodedTextSize);
        encodeBase64.insert(0, "Authorization: Basic ");
        headers = curl_slist_append(headers, encodeBase64.c_str());
        //headers = curl_slist_append(headers, "Cookie: PHPSESSID=a9d73278d9282546512bdcd930917ec8; ct_cookies_test=%7B%22cookies_names%22%3A%5B%5D%2C%22check_value%22%3A%2291b62bbb9d33203ae17f9f3591f82d9e%22%7D; ct_checkjs=0; BITRIX_SM_SALE_UID=14334340; BITRIX_SM_LOGIN=Roga%40gmail.com; BITRIX_SM_SOUND_LOGIN_PLAYED=Y");
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
        curlCode = curl_easy_perform(curl);
        if (curlCode != CURLE_OK) {
            /* ErrorHandler::setErrorMessage((stderr, "curl_easy_perform() failed: %s\n",
                  curl_easy_strerror(curlCode)));*/
        }
        else {
            printf("%lu bytes retrieved\n", (unsigned long)chunk.size);
        }

        if (url_auth != "")
        {
            this->getWebPage(url_auth.c_str());
        }
        //curl_easy_cleanup(curl);
    }

    WebWorker::~WebWorker()
    {
        if (chunk.size > 0)
            free(chunk.memory);
        curl_easy_cleanup(curl);
    }

    MemoryStruct WebWorker::getWebPage(const char* url)
    {
        free(chunk.memory);
        chunk.size = 0;
        void* chunk_void = malloc(1);
        chunk.memory = static_cast<char*>(chunk_void);
        curl_easy_setopt(curl, CURLOPT_URL, url);
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void*)&chunk);
        curl_easy_setopt(curl, CURLOPT_COOKIEFILE, "cookies.txt");
        curl_easy_setopt(curl, CURLOPT_FAILONERROR, true);
        curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);

        curlCode = curl_easy_perform(curl);
        if (curlCode != CURLE_OK) {
            std::string code1 = std::string(curl_easy_strerror(curlCode));
            long res_code = 0;
            curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &res_code);
            std::cerr << code1 + " " + std::to_string(res_code) << std::endl;
            //ErrorHandler::setErrorMessage(code1 + " " + std::to_string(res_code));
        }
        else {
            printf("%lu bytes retrieved\n", (unsigned long)chunk.size);
        }
        return chunk;
    }

    size_t WebWorker::WriteMemoryCallback(void* contents, size_t size, size_t nmemb, void* userp)
    {
        size_t realsize = size * nmemb;
        struct MemoryStruct* mem = (struct MemoryStruct*)userp;
        void* ptr_void = realloc(mem->memory, mem->size + realsize + 1);
        char* ptr = static_cast<char*>(ptr_void);
        if (!ptr) {
            printf("not enough memory (realloc returned NULL)\n");
            return 0;
        }
        mem->memory = ptr;
        memcpy(&(mem->memory[mem->size]), contents, realsize);
        mem->size += realsize;
        mem->memory[mem->size] = 0;
        return realsize;

    }

    size_t writeToFile(void* ptr, size_t size, size_t nmemb, FILE* stream) {
        size_t written = fwrite(ptr, size, nmemb, stream);
        return written;
    }

    bool WebWorker::download_file(const char* url, const char* filename)
    {
        FILE* stream;
        errno_t err = fopen_s(&stream, filename, "wb");
        if (err != 0)
        {
            printf("!!! Failed to create file on the disk\n");
            return false;
        }
        curl_easy_setopt(curl, CURLOPT_URL, url);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, stream);
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeToFile);

        curlCode = curl_easy_perform(curl);
        if (curlCode)
        {
            fclose(stream);
            printf("!!! Failed to download: %s\n", url);
            return false;
        }

        long res_code = 0;
        curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &res_code);
        if (!((res_code == 200 || res_code == 201) && curlCode != CURLE_ABORTED_BY_CALLBACK))
        {
            fclose(stream);
            std::string code1 = std::string(curl_easy_strerror(curlCode));
            curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &res_code);
            //ErrorHandler::setErrorMessage( "Tried download image error:" + code1 + " " + std::to_string(res_code));
            return false;
        }
        if (stream)
        {
            err = fclose(stream);
            if (err != 0)
            {
                printf("The file was not closed\n");
            }
        }
        return true;
    }
}
