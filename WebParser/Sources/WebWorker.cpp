#include "WebWorker.h"

WebWorker::WebWorker()
{
    curl = curl_easy_init();
}

WebWorker::WebWorker(std::string userAgent)
{
    curl = curl_easy_init();
    this->userAgent = userAgent;
}

WebWorker::WebWorker(std::string userAgent, bool setCookies)
{
    curl = curl_easy_init();
    curl_easy_setopt(curl, CURLOPT_COOKIEJAR, "cookies.txt");
    curl_easy_setopt(curl, CURLOPT_COOKIEFILE, "cookies.txt");
    this->userAgent = userAgent;
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



MemoryStruct  WebWorker::postToWebPage(const char* url, std::map<std::string, std::string> files)
{
    free(chunk.memory);
    curl_mime* form = NULL;
    curl_mimepart* field = NULL;
    void* chunk_void = malloc(1);
    chunk.memory = static_cast<char*>(chunk_void);
    chunk.size = 0;
    form = curl_mime_init(curl);
    for (const auto [key, value] : files)
    {
        field = curl_mime_addpart(form);
        curl_mime_name(field, key.c_str());
        curl_mime_filedata(field, value.c_str());
    }
    curl_easy_setopt(curl, CURLOPT_URL, url);
    curl_easy_setopt(curl, CURLOPT_MIMEPOST, form);
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void*)&chunk);
    curlCode = curl_easy_perform(curl);
    if (curlCode != CURLE_OK)
        fprintf(stderr, "curl_easy_perform() failed: %s\n",
            curl_easy_strerror(curlCode));
    curl_mime_free(form);
    return chunk;
}


MemoryStruct  WebWorker::postToWebPage(const char* url, std::string fields)
{

    free(chunk.memory);
    void* chunk_void = malloc(1);
    chunk.memory = static_cast<char*>(chunk_void);
    chunk.size = 0;
    struct curl_slist* headerlist = NULL;
    static const char buf[] = "Content-Type:application/x-www-form-urlencoded;charset=utf-8";

    curl_easy_setopt(curl, CURLOPT_URL, url);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void*)&chunk);
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0);
    headerlist = curl_slist_append(headerlist, buf);
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headerlist);

    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, fields.c_str());
    curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, fields.size());

    CURLcode res = curl_easy_perform(curl);
    if (res != CURLE_OK)
    {
        std::cout << curl_easy_strerror(res) << std::endl;
    }
    if (chunk.memory != NULL && strstr(chunk.memory, "error") != NULL) {
        //ErrorHandler::setErrorMessage(chunk.memory);
    }
    return chunk;

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



bool WebWorker::ftpLoad(std::string pathToImage, std::string pathToLoad)
{
    CURLcode res;
    FILE* hd_src;
    struct stat file_info;
    unsigned long fsize;

    struct curl_slist* headerlist = NULL;
    std::string url = "ftp://ruvip54.hostiman.ru/" + pathToLoad;
    std::string header = "RNFR "  + pathToLoad;

    /* get the file size of the local file */
    if (stat(pathToImage.c_str(), &file_info)) {
        //ErrorHandler::setErrorMessage("error cannot open file when try upload by ftp");
        return false;
    }
    fsize = (unsigned long)file_info.st_size;


    errno_t err = fopen_s(&hd_src, pathToImage.c_str(), "rb");

    /* In windows, this will init the winsock stuff */

    if (curl) {
        /* build a list of commands to pass to libcurl */
        headerlist = curl_slist_append(headerlist, header.c_str());

        curl_easy_setopt(curl, CURLOPT_USERPWD, "image_loader:M2MfpcQXITrfBhj7");
        /* we want to use our own read function */
        curl_easy_setopt(curl, CURLOPT_READFUNCTION, read_callback);
        /* now specify which file to upload */
        curl_easy_setopt(curl, CURLOPT_READDATA, hd_src);
        /* enable uploading */
        curl_easy_setopt(curl, CURLOPT_UPLOAD, 1L);


        /* specify target */
        url = urlEscapeWhiteSpaces(url);
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());

        /* pass in that last of FTP commands to run after the transfer */
        //curl_easy_setopt(curl, CURLOPT_POSTQUOTE, headerlist);

        curl_easy_setopt(curl, CURLOPT_FTP_CREATE_MISSING_DIRS,
            (long)CURLFTP_CREATE_DIR_RETRY);

        /* Set the size of the file to upload (optional).  If you give a *_LARGE
           option you MUST make sure that the type of the passed-in argument is a
           curl_off_t. If you use CURLOPT_INFILESIZE (without _LARGE) you must
           make sure that to pass in a type 'long' argument. */
        curl_easy_setopt(curl, CURLOPT_INFILESIZE_LARGE,
            (curl_off_t)fsize);

        /* Now run off and do what you have been told! */
        curlCode = curl_easy_perform(curl);
        /* Check for errors */
        if (curlCode != CURLE_OK)
        {
           //ErrorHandler::setErrorMessage("curl_easy_perform() failed:" + std::string(curl_easy_strerror(curlCode)));
            return false;
        }


        /* clean up the FTP commands list */
        curl_slist_free_all(headerlist);

    }
    fclose(hd_src); /* close the local file */


    return true;


    //CURL* curl;
    //CURLcode res;
    //FILE* hd_src;
    //struct stat file_info;
    //unsigned long fsize;

    //struct curl_slist* headerlist = NULL;
    //static const char buf_1[] = "RNFR " UPLOAD_FILE_AS;

    ///* get the file size of the local file */
    //if (stat(LOCAL_FILE, &file_info)) {
    //    return ;
    //}
    //fsize = (unsigned long)file_info.st_size;

    //printf("Local file size: %lu bytes.\n", fsize);

    ///* get a FILE * of the same file */
    //FILE* stream;
    //errno_t err = fopen_s(&hd_src, LOCAL_FILE, "rb");

    /////* In windows, this will init the winsock stuff */
    //curl_global_init(CURL_GLOBAL_ALL);

    ///* get a curl handle */
    //curl = curl_easy_init();
    //if (curl) {
    //    /* build a list of commands to pass to libcurl */
    //    headerlist = curl_slist_append(headerlist, buf_1);

    //    /* we want to use our own read function */
    //    curl_easy_setopt(curl, CURLOPT_READFUNCTION, read_callback);

    //    /* enable uploading */
    //    curl_easy_setopt(curl, CURLOPT_UPLOAD, 1L);

    //    /* specify target */
    //    curl_easy_setopt(curl, CURLOPT_URL, REMOTE_URL);

    //    /* pass in that last of FTP commands to run after the transfer */
    //    curl_easy_setopt(curl, CURLOPT_POSTQUOTE, headerlist);

    //    /* now specify which file to upload */
    //    curl_easy_setopt(curl, CURLOPT_READDATA, hd_src);
    //    curl_easy_setopt(curl, CURLOPT_USERPWD, "image_loader:M2MfpcQXITrfBhj7");

    //    /* Set the size of the file to upload (optional).  If you give a *_LARGE
    //       option you MUST make sure that the type of the passed-in argument is a
    //       curl_off_t. If you use CURLOPT_INFILESIZE (without _LARGE) you must
    //       make sure that to pass in a type 'long' argument. */
    //    curl_easy_setopt(curl, CURLOPT_INFILESIZE_LARGE,
    //        (curl_off_t)fsize);

    //    /* Now run off and do what you have been told! */
    //    res = curl_easy_perform(curl);
    //    /* Check for errors */
    //    if (res != CURLE_OK)
    //        fprintf(stderr, "curl_easy_perform() failed: %s\n",
    //            curl_easy_strerror(res));

    //    /* clean up the FTP commands list */
    //    curl_slist_free_all(headerlist);

    //    /* always cleanup */
    //    curl_easy_cleanup(curl);
    //}
    //fclose(hd_src); /* close the local file */

    //curl_global_cleanup();
    //return ;
}

//size_t WebWorker::read_callback(char* ptr, size_t size, size_t nmemb, void* userdata)
//{
//    FILE* readhere = (FILE*)userdata;
//    curl_off_t nread;
//
//    /* copy as much data as possible into the 'ptr' buffer, but no more than
//       'size' * 'nmemb' bytes! */
//    size_t retcode = fread_s(ptr, size, nmemb,sizeof(char), readhere);
//
//    nread = (curl_off_t)retcode;
//
//    fprintf(stderr, "*** We read %" CURL_FORMAT_CURL_OFF_T
//        " bytes from file\n", nread);
//    return retcode;
//}

// Callback function to read data from the local file
size_t WebWorker::read_callback(void* ptr, size_t size, size_t nmemb, void* userdata) {
    FILE* file = static_cast<FILE*>(userdata);
    size_t retcode = fread(ptr, size, nmemb, file);
    return retcode;
}

std::string WebWorker::urlEscape(std::string text)
{
    return curl_easy_escape(curl, text.c_str(), text.size());
}
std::string  WebWorker::urlEscapeWhiteSpaces(std::string text)
{
    size_t spaces_pos = 0;
    while ((spaces_pos = text.find(" ")) != std::string::npos)
        text.replace(spaces_pos, 1, "%20");
    return text;
}

std::string WebWorker::removeHtmlTags(std::string text)
{
    //&#40;     (
    //&#41;     )
    //&quot;    "
    //&nbsp;    space
    size_t text_limitation = 99999; //vk limitation
    size_t left_br = 0;
    size_t right_br = 0;
    size_t nbsp = 0;
    size_t newLine = 0;
    size_t quot = 0;
    size_t lt = 0;
    size_t gt = 0;
    size_t percent = 0;
    size_t plus = 0;
    size_t singleQuote = 0;
    while ((left_br = text.find("&#40;")) != std::string::npos && left_br <= text_limitation)
        text.replace(left_br, 5, "(");
    while ((right_br = text.find("&#41;")) != std::string::npos && right_br <= text_limitation)
        text.replace(right_br, 5, ")");
    while ((nbsp = text.find("&nbsp;")) != std::string::npos && nbsp <= text_limitation)
        text.replace(nbsp, 6, " ");
    while ((quot = text.find("&quot;")) != std::string::npos && quot <= text_limitation)
        text.replace(quot, 6, "\"");
    while ((lt = text.find("&lt;")) != std::string::npos && lt <= text_limitation)
        text.replace(lt, 4, "<");
    while ((gt = text.find("&gt;")) != std::string::npos && gt <= text_limitation)
        text.replace(gt, 4, ">");
    while ((percent = text.find("&#37;")) != std::string::npos && percent <= text_limitation)
        text.replace(percent, 5, "%");
    while ((plus = text.find("&#43;")) != std::string::npos && plus <= text_limitation)
        text.replace(plus, 5, "+");
    while ((singleQuote = text.find("&#39;")) != std::string::npos && singleQuote <= text_limitation)
        text.replace(singleQuote, 5, "\'");

    return text;
}