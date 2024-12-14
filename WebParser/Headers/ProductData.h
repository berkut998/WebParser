#pragma once
#include <string>
namespace {
	class ProductData final
	{
	public:
		std::string name;
		std::string url;
		std::string pathToImage;
		std::string price;
		std::string description;
		std::string category;
		std::string image_url;
		std::string product_id;
		std::string manufacturer;
		int category_id = 0;
		bool stock_status = true;
	};
}

