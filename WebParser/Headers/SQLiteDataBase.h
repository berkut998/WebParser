#pragma once

#include <string>
#include <sys/stat.h>
#include <string>
#include <fstream>
#include <vector>

#include "sqlite3.h"
#include "Image.h"
#include "ProductData.h"
#include "MemoryStruct.h"

/*
* TODO:
* replace MemoryStruct to Image struct both better not use 
*/
namespace WP
{
	class SQLiteDataBase
	{
	public:
		SQLiteDataBase(const char* pathToDataBase, int& errorCode, std::string& error);
		~SQLiteDataBase();
		bool createDataBase(const char* pathToDataBase);
		static bool dataBaseFileExist(const char* pathToDataBase);
		ProductData getProduct(std::string  product_url);
		Image* getImage(int productIdOnSite, int siteId);
		bool addProduct(ProductData product, int siteId);
		bool updateProduct(ProductData product, int siteId);
		bool productExist(std::string url);


	private:
		//add image to table "productToImage"
		bool addImage(int productId, std::string pathToImage);
		//add product to table "products"
		bool addProductData(ProductData product, int siteId);
		//retrun productId from table "products" if not found return 0
		int getProductId(int productIdOnSite, int siteId);
		bool createProductTable();
		//different table because theoretically product can have more than 1 image
		bool createProductToImageTable();
		bool createIndexForProdutcTable();
		Image* getImageBuffer(std::string pathToImage);

		std::string convertUnsignedCharToString(const unsigned char* str);
		sqlite3* db;
		sqlite3_stmt* queryResult = nullptr;
	};
}


