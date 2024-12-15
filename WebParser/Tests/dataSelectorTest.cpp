#define BOOST_TEST_MODULE dataSelectorTest 
#pragma once
#include <boost/test/unit_test.hpp>
#include <string>
#include "DataSelector.h"


/*
		std::vector<std::string> selectDataFromHTML(std::vector<std::vector<std::string>> tags, std::string_view text);
		std::string removeHTMLTags(std::string text);
		std::string removeDoubleSpacesAndLF(std::string text);
		std::string removeDublicateRow(std::string text);
*/


BOOST_AUTO_TEST_CASE(test_removeHTMLTags)
{
	DataSelector ds = DataSelector();
	std::string clearStr = ds.removeHTMLTags("<h1>a<h1>");
	BOOST_TEST(clearStr == "a");
}