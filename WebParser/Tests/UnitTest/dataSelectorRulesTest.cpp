#define BOOST_TEST_MODULE dataSelectorRuleTest
#pragma once
#include <boost/test/unit_test.hpp>
#include <DataSelectorRules.h>
#include <fstream>
#include <string>

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

	WP::DataSelectorRules dataSelectRules = WP::DataSelectorRules();
	dataSelectRules.load("testSitePatterns.com");
	BOOST_TEST(dataSelectRules.getRule("ref")[0][0] == "favorite ajax_trigger");
	BOOST_TEST(dataSelectRules.getRule("ref")[0][1] == "<a href=\"");
	BOOST_TEST(dataSelectRules.getRule("ref")[0][2] == "\" class=\"name\">");
	std::remove("testSitePatterns.com.txt");
}