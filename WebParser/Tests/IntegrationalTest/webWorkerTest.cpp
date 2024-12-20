#define BOOST_TEST_MODULE webWorkerTest
#pragma once
#include "WebWorker.h"
#include <boost/test/unit_test.hpp>
#include <string>
#include <fstream>
const std::string userAgent = "Mozilla / 5.0 (Windows NT 6.1; WOW64; rv:57.0) Gecko / 20100101 Firefox / 57.0";
const std::string textExpected = u8R"(<!doctype html>
<html>
<head>
    <title>Example Domain</title>

    <meta charset="utf-8" />
    <meta http-equiv="Content-type" content="text/html; charset=utf-8" />
    <meta name="viewport" content="width=device-width, initial-scale=1" />
    <style type="text/css">
    body {
        background-color: #f0f0f2;
        margin: 0;
        padding: 0;
        font-family: -apple-system, system-ui, BlinkMacSystemFont, "Segoe UI", "Open Sans", "Helvetica Neue", Helvetica, Arial, sans-serif;
        
    }
    div {
        width: 600px;
        margin: 5em auto;
        padding: 2em;
        background-color: #fdfdff;
        border-radius: 0.5em;
        box-shadow: 2px 3px 7px 2px rgba(0,0,0,0.02);
    }
    a:link, a:visited {
        color: #38488f;
        text-decoration: none;
    }
    @media (max-width: 700px) {
        div {
            margin: 0 auto;
            width: auto;
        }
    }
    </style>    
</head>

<body>
<div>
    <h1>Example Domain</h1>
    <p>This domain is for use in illustrative examples in documents. You may use this
    domain in literature without prior coordination or asking for permission.</p>
    <p><a href="https://www.iana.org/domains/example">More information...</a></p>
</div>
</body>
</html>
)";

BOOST_AUTO_TEST_CASE(testGetWebPage)
{
	WP::WebWorker webWorker = WP::WebWorker(userAgent,true);
	WP::MemoryStruct mem = webWorker.getWebPage("https://example.com");
    std::string currString = std::string(mem.memory);
    BOOST_TEST(textExpected == currString);
}

BOOST_AUTO_TEST_CASE(downloadFileTest)
{
    const std::string fileName = "testDownload";
    WP::WebWorker webWorker = WP::WebWorker(userAgent, true);
    webWorker.download_file("https://example.com", fileName.c_str());
    std::ifstream file(fileName);
    std::string curData = "";
    std::stringstream buffer;
    if (file.is_open())
    {
        buffer << file.rdbuf();
        file.close();
    }
    BOOST_TEST(textExpected == buffer.str());
}

//because authenticationtest.com use pure form-data type without base 64 cannot test while not refactor code 
// to work properly with encoding and without it
BOOST_AUTO_TEST_CASE(authTest)
{
    WP::WebWorker webWorker = WP::WebWorker(userAgent, true);
    const std::string login = "simpleForm@authenticationtest.com";
    const std::string password = "pa$$w0rd";
    const std::string url = "https://authenticationtest.com//login/?mode=simpleFormAuth";
    //webWorker.auth(login, password, url, url);
}


// https://authenticationtest.com/simpleFormAuth/
//; 