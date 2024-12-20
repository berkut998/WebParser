#define BOOST_TEST_MODULE sqliteDataBaseTest 
#pragma once
#include <boost/test/unit_test.hpp>
#include "SQLiteDataBase.h"
#include "ProductData.h"
#include <string>

const std::string dbName = "testDB.db";

BOOST_AUTO_TEST_CASE(test_openDBAndExist)
{
	int errorCode = 0;
	std::string errorMessage = "";
	WP::SQLiteDataBase sqliteDB = WP::SQLiteDataBase(dbName.c_str(), errorCode, errorMessage);
	bool dbCreated = WP::SQLiteDataBase::dataBaseFileExist(dbName.c_str());
	BOOST_TEST(dbCreated == true);
	BOOST_TEST(errorCode == 0);
	BOOST_TEST(errorMessage == "");
	std::remove(dbName.c_str());
}

BOOST_AUTO_TEST_CASE(test_dbExist)
{
	bool dbExist = WP::SQLiteDataBase::dataBaseFileExist(dbName.c_str());
	if (dbExist == false)
	{
		int errorCode = 0;
		std::string errorMessage = "";
		WP::SQLiteDataBase sqliteDB = WP::SQLiteDataBase(dbName.c_str(), errorCode, errorMessage);
		sqliteDB.dataBaseFileExist(dbName.c_str());
	}
	dbExist = WP::SQLiteDataBase::dataBaseFileExist(dbName.c_str());
	BOOST_TEST(dbExist == true);
	std::remove(dbName.c_str());
}

WP::ProductData crateProduct()
{
	WP::ProductData product = WP::ProductData();
	product.name = "HereName";
	product.url = "Hereurl";
	product.pathToImage = "HerepathToImage";
	product.price = "100";
	product.description = "Heredescription";
	product.image_url = "Hereimage_url";
	product.product_id = "1233333333";
	product.manufacturer = "Heremanufacturer";
	return product;
}

BOOST_AUTO_TEST_CASE(test_addAndGetProduct)
{
	int errorCode = 0;
	std::string errorMsg = "";
	WP::SQLiteDataBase db = WP::SQLiteDataBase(dbName.c_str(), errorCode, errorMsg);
	db.createDataBase(dbName.c_str());
	WP::ProductData product = crateProduct();
	bool res = db.addProduct(product, 0);
	BOOST_TEST(res == false);//because image
	WP::ProductData product2 = db.getProduct(product.url);
	BOOST_TEST(product == product2);
	std::remove("dataBase.db");
}

BOOST_AUTO_TEST_CASE(test_addGetUpdateProduct)
{
	int errorCode = 0;
	std::string errorMsg = "";
	WP::SQLiteDataBase db = WP::SQLiteDataBase(dbName.c_str(), errorCode, errorMsg);
	db.createDataBase(dbName.c_str());
	WP::ProductData product = crateProduct();
	bool res = db.addProduct(product, 0);
	BOOST_TEST(res == false); //because image
	WP::ProductData product2 = db.getProduct(product.url);
	BOOST_TEST(product == product2);
	product2.price = "150";
	db.updateProduct(product2,0);
	WP::ProductData product3 = db.getProduct(product2.url);
	BOOST_TEST(product3 != product2);
	std::remove("dataBase.db");
}

BOOST_AUTO_TEST_CASE(test_addExistProduct)
{
	int errorCode = 0;
	std::string errorMsg = "";
	WP::SQLiteDataBase db = WP::SQLiteDataBase(dbName.c_str(), errorCode, errorMsg);
	db.createDataBase(dbName.c_str());
	WP::ProductData product = crateProduct();
	bool res = db.addProduct(product, 0);
	BOOST_TEST(res == false);//because image
	db.productExist(product.url);
	std::remove("dataBase.db");
}

BOOST_AUTO_TEST_CASE(test_getImage)
{
	int errorCode = 0;
	std::string errorMsg = "";
	WP::SQLiteDataBase db = WP::SQLiteDataBase(dbName.c_str(), errorCode, errorMsg);
	db.createDataBase(dbName.c_str());
	WP::ProductData product = crateProduct();
	bool res = db.addProduct(product, 0);
	BOOST_TEST(res == false);//because image
	db.getImage(std::atoi(product.product_id.c_str()), 0);
	std::remove("dataBase.db");
}