#define BOOST_TEST_MODULE webWorkerTest
#pragma once
#include "WebParser.h"
#include <boost/test/unit_test.hpp>
#include <fstream>

const std::string fileName = "agrosemfond.ru";

void createConfigFile()
{
	const std::string configData = u8R"([categories]
[[categoeies1]]
class="sectionTrigger"
[[categoeies2]]
<div class="pre-letter">
<a href="
">
[productCardBlock]
[[productCardBlock1]]
class="catalog-section four_in_row
 Top.Mail.Ru counter 
[productCard]
[[productCardStartEnd]]
<div class="card " itemscope itemtype=
<button type="button" class="card_button to-basket">
[ref]
[[ref1]]
favorite ajax_trigger
<a href="
" class="card_slider swiper">
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
class="about-container">
itemprop="name">
</h1>
[price]
[[price1]]
<span class="price"><span class="product_price">
</span>
[price_catalog]
[[price_catalog1]]
<div class="card_price-actual">
<span itemprop="price">
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
	std::ofstream outFile(fileName+".txt");
	if (outFile.is_open())
	{
		outFile << configData;
		outFile.close();
	}
}

BOOST_AUTO_TEST_CASE(testParseAllPages)
{
	const std::string url = "https://agrosemfond.ru/catalog/semena_ovoshchey/repa/";
	createConfigFile();
	WP::WebParser webParser = WP::WebParser(fileName);
	std::vector<WP::ProductData> products = webParser.parseAllPages(url);
	std::remove("agrosemfond.ru.txt");
	BOOST_TEST(products.size() > 0);
}