#pragma once
#include <iostream>
#include <stdio.h>
#include <string>
#include <thread>
#include <future>
#include <functional>
#include <ctime>
#include <direct.h>
#include <filesystem>
#include <cassert>
#include <exception>
#include <iostream>
#include <sstream>
#include <string>
#include <future>
#include <time.h> 
#include <queue>

#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>

#include "tbb/parallel_for.h"
#include "tbb/blocked_range.h"
#include "tbb/spin_mutex.h"

#include "SitePatterns.h"
#include "ProductData.h"
#include "WebWorker.h"
#include "DataSelector.h"
#include "SitePatternsLoader.h"
//#include "ParsingStatus.h"

//#include "ParsedData.h"
//#include "ErrorHandler.h"
//#include "SQLiteDataBase.h"

namespace
{
	using namespace std;
	class WebParser final
	{

	public:

		enum class Site :int
		{
			unknown,
			agrosemfond,
			seedspost,
			semenazakaz
		};

		WebParser(std::string site);
		WebParser(int margin, std::string site);

		void getSubCategory(string url);
		void parseCategories(std::map<std::string, std::string>);
		std::vector<ProductData> parseAllPages(std::string categoryURL, std::string categoryName);
		std::vector<ProductData> parsePage(std::string webPage, std::string categoryName, std::string pageNumber);
		void cleanUpTMPFolder();
		// add here try catch because it critical for special product


	private:
		Site getSiteId(std::string _site);
		std::string generateURL(std::string startUrl);
		int getNumberOfPages(std::string webPage);
		bool setPageOption(std::string webPage);
		std::vector<ProductData> startParsePage(int pageIndex, WebWorker* webWorker, std::string categoryURL,
			std::string categoryName, MemoryStruct localMem);
		bool urlParamIsValid(std::string_view url);
		std::filesystem::path createPathToImage(std::string categoryName, std::string pageNumber);
		bool  getProductsRefPriceCard(std::vector<std::string>& refs, std::vector<std::string>& prices, std::string webPage);
		std::string getProductCardBlock(std::string webPage);
		std::vector<ProductData> getOldProducts(std::vector<std::string>& refs, std::vector<std::string> prices);
		std::vector<ProductData> getImageForOldProduct(std::string dirImage, std::vector<ProductData>& oldProducts);
		std::vector<ProductData> findOldProductWithImage(std::vector<ProductData> oldProducts,
			std::vector<ProductData> oldProductsWitoutImage);
		std::vector<ProductData> getNewProducts(std::vector<std::string> refs, WebWorker* webWorker, std::string pathToImage);
		ProductData getProductData(WebWorker* webWorker, std::string productUrl, std::string pathToImage);
		void downloadAllImages(WebWorker* webWorker, std::vector<std::string>& refs, std::vector<ProductData>& products,
			std::filesystem::path pathToImageFolder);
		bool downloadImageProduct(WebWorker* webWorker, ProductData& product, std::string pathToImage);
		std::vector<ProductData> updateImagePathDataInNewProduct(std::vector<ProductData>newProducts,
			std::vector<ProductData>allProductWithoutImage);
		void addNewProducts(std::vector<std::string> refs, std::vector<ProductData> products);


		/// <summary>
		/// in percent (15%)
		/// </summary>
		int _margin = 0;
		SitePatterns patterns;
		Site _site = Site::unknown;
		std::string domenName = "https://agrosemfond.ru";
		bool useParallel = false;
		const char* sqliteDataBaseFileName = "ProductDataBase.db";
		std::string tmpFolder = "";

		tbb::spin_mutex mtx;

		queue<int> failProdPage;

	};
}


