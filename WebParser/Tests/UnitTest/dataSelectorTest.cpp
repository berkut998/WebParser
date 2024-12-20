#define BOOST_TEST_MODULE dataSelectorTest 
#pragma once
#include <boost/test/unit_test.hpp>
#include "DataSelector.h"

using namespace WP;

BOOST_AUTO_TEST_CASE(test_removeHTMLTags)
{
	std::string clearStr = WP::DataSelector::removeHTMLTags("<h1>a<h1>");
	BOOST_TEST(clearStr == "\na\n");
	clearStr = WP::DataSelector::removeHTMLTags("<h1>a<h1><>");
	BOOST_TEST(clearStr == "\na\n\n");
	clearStr = WP::DataSelector::removeHTMLTags("<h1>a<h1><s>");
	BOOST_TEST(clearStr == "\na\n\n");
	clearStr = WP::DataSelector::removeHTMLTags("<h1>a<h1><<");
	BOOST_TEST(clearStr == "\na\n\n");
	clearStr = WP::DataSelector::removeHTMLTags("<h1><a<h1>");
	BOOST_TEST(clearStr == "\n\n");
};

BOOST_AUTO_TEST_CASE(test_removeDoubleSpacesAndLF)
{
	std::string clearStr = WP::DataSelector::removeDoubleSpacesAndLF("     a\n\n\n\n\n\n");
	BOOST_TEST(clearStr == " a\n");
	clearStr = WP::DataSelector::removeDoubleSpacesAndLF(" \n \n \n \n");
	BOOST_TEST(clearStr == "\n");
	clearStr = WP::DataSelector::removeDoubleSpacesAndLF(" \n \n \n \na  \n");
	BOOST_TEST(clearStr == "\na\n");
}

BOOST_AUTO_TEST_CASE(test_removeDublicateRow)
{
	std::string  clearStr = WP::DataSelector::removeDublicateRow("aaa\naaa\naaa\nbbb");
	BOOST_TEST(clearStr == "aaa\nbbb");
}


BOOST_AUTO_TEST_CASE(load_test)
{
	std::string textSettings = u8R"([categories]
[[categoeies1]]
class="sectionTrigger"
[[categoeies2]]
<div class="pre-letter">
<a href="
">
[productCard]
[[productCardStartEnd]]
class="block itemCardID onion
class="params-container"
[ref]
[[ref1]]
favorite ajax_trigger
<a href="
" class="name">
[Pagination]
[[Pagination1]]
<div class="bx-pagination-container row">
[[Pagination2]]
<li class="">
"><span>
</span>
[Page]
PAGEN_1=
[name]
[[name1]]
itemprop="name">
</h1>
[price]
[[price1]]
itemprop="price">
</span>
[price_catalog]
[[price_catalog1]]
price_string
class="middle_size"
itemprop="price">
</span>
[[price_catalog2]]
||
price_block_simple
"price">
</span>
[[price_catalog3]]
||
price_block_crossed
class="old_price">
руб.
[image]
[[image1]]
class="main-image"
<img src="
" 
[description]
[[description1]]
class="tabs-head"
class="tab">
</div>
[[description2]]
class="tabs-content"
itemprop="description">
</div>
[[description3]]
||
class="tabs-head"
<div class="tab">Описание</div>
<div class="tab">Посадка и уход</div>
[[description4]]
ordinary-params">
<div class="ordinary-params__item">
<div class="tab">
[[description5]]
||
class="tabs-head"
<div class="tab" id="params_tab">Характеристики</div>
Отзывы</div>
[[description6]]
itemprop="description">
class="ordinary-params"
<div class="tab">
<span>Комментарии</span>
[productID]
[[productID1]]
class="triggers"
<a attr-id="
" href="
[categoryName]
<div class="string">
<a href="
">
</a>
[manufacturer]
[[manufacturer1]]
Производитель
<div class="value">
</div>
[productIsNotAvailable]
[[productIsNotAvailable]]
show_modal_request_entrance_goods
class="request_show_modal)";
	std::remove("testSitePatterns.com.txt");
	std::ofstream fs("testSitePatterns.com.txt");
	fs << textSettings;
	fs.close();

	std::unordered_map<std::string, std::vector<std::vector<std::string>>> rules =  WP::DataSelector::loadRules("testSitePatterns.com");
	WP::DataSelector dataSelectRules = WP::DataSelector(rules);
	BOOST_TEST(rules["ref"][0][0] == "favorite ajax_trigger");
	BOOST_TEST(rules["ref"][0][1] == "<a href=\"");
	BOOST_TEST(rules["ref"][0][2] == "\" class=\"name\">");
	std::remove("testSitePatterns.com.txt");
}