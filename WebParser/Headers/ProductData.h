#pragma once
#include <string>
#include <iostream>

namespace WP
{
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


		bool operator!=(const ProductData& other) const
		{
			if (name != other.name || url != other.url || pathToImage != other.pathToImage ||
				price != other.price || description != other.description || category != other.category ||
				image_url != other.image_url || product_id != other.product_id || manufacturer != other.manufacturer ||
				category_id != other.category_id || stock_status != other.stock_status)
				return true;
			return false;
		}
		bool operator==(const ProductData& other) const
		{
			if (name == other.name && url == other.url && pathToImage == other.pathToImage &&
				price == other.price && description == other.description && category == other.category &&
				image_url == other.image_url && product_id == other.product_id && manufacturer == other.manufacturer &&
				category_id == other.category_id && stock_status == other.stock_status)
				return true;
			return false;
		}

		friend std::ostream& operator<<(std::ostream& os, const ProductData& pd)
		{
			os << pd.product_id;
			return os;
		}
	};
}


