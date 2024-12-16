#define BOOST_TEST_MODULE dataSelectorTest 
#pragma once
#include <boost/test/unit_test.hpp>
#include "DataSelector.h"

using namespace DataSelect;

/*
		
		std::string removeHTMLTags(std::string text);

		std::string removeDoubleSpacesAndLF(std::string text);
		std::string removeDublicateRow(std::string text);
		std::vector<std::string> selectDataFromHTML(std::vector<std::vector<std::string>> tags, std::string_view text);
*/


BOOST_AUTO_TEST_CASE(test_removeHTMLTags)
{
	DataSelector ds = DataSelector();
	std::string clearStr = ds.removeHTMLTags("<h1>a<h1>");
	BOOST_TEST(clearStr == "\na\n");
	clearStr = ds.removeHTMLTags("<h1>a<h1><>");
	BOOST_TEST(clearStr == "\na\n\n");
	clearStr = ds.removeHTMLTags("<h1>a<h1><s>");
	BOOST_TEST(clearStr == "\na\n\n");
	clearStr = ds.removeHTMLTags("<h1>a<h1><<");
	BOOST_TEST(clearStr == "\na\n\n");
	clearStr = ds.removeHTMLTags("<h1><a<h1>");
	BOOST_TEST(clearStr == "\n\n");
};

BOOST_AUTO_TEST_CASE(test_removeDoubleSpacesAndLF)
{
	DataSelector ds = DataSelector();
	std::string clearStr = ds.removeDoubleSpacesAndLF("     a\n\n\n\n\n\n");
	BOOST_TEST(clearStr == " a\n");
	clearStr = ds.removeDoubleSpacesAndLF(" \n \n \n \n");
	BOOST_TEST(clearStr == "\n");
	clearStr = ds.removeDoubleSpacesAndLF(" \n \n \n \na  \n");
	BOOST_TEST(clearStr == "\na\n");
}

BOOST_AUTO_TEST_CASE(test_removeDublicateRow)
{
	DataSelector ds = DataSelector();
	std::string  clearStr = ds.removeDublicateRow("aaa\naaa\naaa\nbbb");
	BOOST_TEST(clearStr == "aaa\nbbb");
}