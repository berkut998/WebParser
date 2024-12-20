#include "WebParser.h"

namespace WP
{
	WebParser::WebParser(std::string site)
	{
		parseRules = WP::DataSelector::loadRules(site);
		//ParsingStatus::getInstance().clearData();
		domenName = "https://" + site;
		_site = getSiteId(site);
		//ParsingStatus::getInstance().clearData();
	}

	WebParser::Site  WebParser::getSiteId(std::string _site)
	{
		if (_site == "agrosemfond.ru")
			return Site::agrosemfond;
		else if (_site == "seedspost.ru")
			return Site::seedspost;
		else if (_site == "semena-zakaz.ru")
			return Site::semenazakaz;
		return  Site::unknown;
	}

	void WebParser::getSubCategory(string url)
	{
		WebWorker webWorker = WebWorker("Mozilla / 5.0 (Windows NT 6.1; WOW64; rv:57.0) Gecko / 20100101 Firefox / 57.0");
		MemoryStruct mem = webWorker.getWebPage(url.c_str());
		DataSelector dataSelector = DataSelector(parseRules);
		std::vector<std::string> categories = dataSelector.selectDataFromHTML("categories", std::string(mem.memory));
		std::vector<std::string> categoriesName = dataSelector.selectDataFromHTML("categoryName", std::string(mem.memory));
		if (categories.size() != categoriesName.size())
			//ErrorHandler::setErrorMessage(u8"Не удалось выбрать все категории");
			for (size_t i = 0; i < categories.size(); i++)
			{
				//ParsedData::getInstance().categoryName.insert({ categoriesName[i], categories[i] });
			}
	}

	void WebParser::parseCategories(std::map<std::string, std::string> categories)
	{
		// righ now useless function 
		//ParsingStatus::getInstance().setTotalCategories((int)categories.size());
		//for (const auto [key, value] : categories)
		//{
		//	std::string url = domenName + value;
		//	parseAllPages(url, key);
		//	//ParsingStatus::getInstance().setParsedCategories(1);
		//}
	}


	std::vector<ProductData> WebParser::parseAllPages(std::string categoryURL)
	{
		WebWorker webWorker = WebWorker("Mozilla / 5.0 (Windows NT 6.1; WOW64; rv:57.0) Gecko / 20100101 Firefox / 57.0");
		categoryURL = generateURL(categoryURL);
		MemoryStruct mem = webWorker.getWebPage(categoryURL.c_str());
		boost::uuids::uuid uuid = boost::uuids::random_generator()();
		tmpFolder = boost::uuids::to_string(uuid);
		int pageCount = getNumberOfPages(std::string(mem.memory));
		std::vector<ProductData> products;
		if (useParallel && pageCount > 1)
			tbb::parallel_for(1, pageCount + 1, [&](int i) {
			WebWorker localWebWorker = WebWorker("Mozilla / 5.0 (Windows NT 6.1; WOW64; rv:57.0) Gecko / 20100101 Firefox / 57.0");
			MemoryStruct localMem = mem;
			std::vector<ProductData> currPageProducts = startParsePage(i, &localWebWorker, categoryURL, tmpFolder, localMem);
			{
				tbb::spin_mutex::scoped_lock lock(mtx);
				products.insert(products.end(), currPageProducts.begin(), currPageProducts.end());
			}
				});

		else
			for (int i = 1; i <= pageCount; i++)
			{
				std::vector<ProductData> currPageProducts = startParsePage(i, &webWorker, categoryURL, tmpFolder, mem);
				products.insert(products.end(), currPageProducts.begin(), currPageProducts.end());
			}

		while (!failProdPage.empty())
		{
			int pageNumber = failProdPage.front();
			std::vector<ProductData> currPageProducts = startParsePage(pageNumber, &webWorker, categoryURL, tmpFolder, mem);
			products.insert(products.end(), currPageProducts.begin(), currPageProducts.end());
			failProdPage.pop();
		}
		return products;
	}

	std::string WebParser::generateURL(std::string startUrl)
	{
		if (parseRules.count("productQuantityOnPage") == 1)
			startUrl = startUrl + '?' + parseRules["productQuantityOnPage"].back().back() + "&";
		else
			startUrl = startUrl + '?';
		return startUrl;
	}
	int WebParser::getNumberOfPages(std::string webPage)
	{
		int pageCount = 1;
		DataSelector dataSelector = DataSelector(parseRules);
		std::vector<std::string> allPages = dataSelector.selectDataFromHTML("Pagination", webPage);
		bool pageOptionIsSet = true;
		if (parseRules.count("PageStart") == 1)
			pageOptionIsSet = setPageOption(webPage);
		if (allPages.size() != 0 && pageOptionIsSet)
		{
			pageCount = std::stoi(allPages.back());
		}
		//ParsingStatus::getInstance().setTotalPages(pageCount);
		return pageCount;
	}

	bool WebParser::setPageOption(std::string webPage)
	{
		DataSelector dataSelector = DataSelector(parseRules);
		std::vector<std::string> pageOption = dataSelector.selectDataFromHTML("PageStart", webPage);
		if (pageOption.size() == 0)
		{
			//ErrorHandler::setErrorMessage("Cann`t get PageStart. Supossed what only 1 page exist in this category.");
			return false;
		}
		std::vector<std::vector<std::string>> pageOptionsVec;
		pageOptionsVec.push_back(pageOption);
		parseRules["Page"] = pageOptionsVec;
		return true;
	}

	std::vector<ProductData> WebParser::startParsePage(int pageIndex, WebWorker* webWorker, std::string categoryURL,
		std::string categoryName, MemoryStruct localMem)
	{
		std::string strPageIndex = to_string(pageIndex);
		std::vector<ProductData> currPageProducts;
		std::string pageURL = categoryURL;
		if (pageIndex > 1)
		{
			pageURL = categoryURL + parseRules["Page"].back().back() + strPageIndex;
			if (urlParamIsValid(pageURL) == false)
				return currPageProducts;
			localMem = webWorker->getWebPage(pageURL.c_str());
		}
		currPageProducts = parsePage(std::string(localMem.memory), categoryName, to_string(pageIndex));
		//CurrentAppStatus::getInstance().newEventHappen(u8"Парсинг страницы окончен " + strPageIndex);
		//ParsingStatus::getInstance().setParsedPages(1);
		if (currPageProducts.size() == 0 && (failProdPage.empty() || (!failProdPage.empty() && failProdPage.front() != pageIndex)))
		{
			//ErrorHandler::setErrorMessage("Product not found " + pageURL);
			tbb::spin_mutex::scoped_lock lock(mtx);
			failProdPage.push(pageIndex);
			std::this_thread::sleep_for(5000ms);
		}
		return currPageProducts;
	}

	bool WebParser::urlParamIsValid(std::string_view url)
	{
		size_t paramQUeryMaskStart = url.find_first_of('?');
		if (paramQUeryMaskStart != std::string::npos)
		{
			int delimCount = 0;
			int keyValueCount = 0;
			std::string_view paramQuery = url.substr(paramQUeryMaskStart, url.size());
			for (size_t i = 0; i < paramQuery.size(); i++)
			{
				if (paramQuery[i] == '&' || paramQuery[i] == ';')
					delimCount++;
				else if (paramQuery[i] == '=')
					keyValueCount++;
			}
			if (keyValueCount - 1 != delimCount)
			{
				//ErrorHandler::setErrorMessage("URL incorrect format:" + std::string(url));
				return false;
			}

		}
		return true;
	}


	std::vector<ProductData> WebParser::parsePage(std::string webPage, std::string categoryName, std::string pageNumber)
	{
		filesystem::path pathToImage = createPathToImage(categoryName, pageNumber);
		std::vector<std::string> refs, prices;
		bool productsWasFound = getProductsRefPriceCard(refs, prices, webPage);
		//ParsingStatus::getInstance().setTotalProducts((int)refs.size());
		std::vector<ProductData> oldProducts;// = getOldProducts(refs, prices);
		std::vector<ProductData> oldProductsWitoutImage;//= getImageForOldProduct(pathToImage.string(), oldProducts);
		std::vector<ProductData> oldProductWithImage;//= findOldProductWithImage(oldProducts, oldProductsWitoutImage);
		//oldProducts.clear();
		WebWorker webWorker = WebWorker("Mozilla / 5.0 (Windows NT 6.1; WOW64; rv:57.0) Gecko / 20100101 Firefox / 57.0");
		std::vector<ProductData> newProducts = getNewProducts(refs, &webWorker, pathToImage.string());

		//hard for reading code need refactoring
		std::vector<ProductData> allProductWithoutImage = newProducts;
		for (const auto& product:oldProductsWitoutImage)
			allProductWithoutImage.push_back(product);
		downloadAllImages(&webWorker, refs, allProductWithoutImage, pathToImage);
		newProducts = updateImagePathDataInNewProduct(newProducts, allProductWithoutImage);
		//addNewProducts(refs, newProducts);
		std::vector<ProductData> allProducts = allProductWithoutImage;
		for (const auto& product : oldProductsWitoutImage)
			allProducts.push_back(product);
		if (allProducts.size() == 0 && productsWasFound == false)
			
			;//ErrorHandler::setErrorMessage("can`t parse page:" + pageNumber + " something went wrong");
		//ParsingStatus::getInstance().setParsedProducts((int)allProducts.size());
			return allProducts;

	}

	filesystem::path  WebParser::createPathToImage(std::string categoryName, std::string pageNumber)
	{
		//this work better than create_directories because spaces ' ' create_directories throw error
		filesystem::path pathToImage = "tmp/";
		std::filesystem::create_directory(pathToImage);
		size_t currPosition = 0;
		while ((currPosition = categoryName.find("\"", currPosition)) != std::string::npos)
		{
			categoryName = categoryName.erase(currPosition, 1);
		}
		currPosition = 0;
		while ((currPosition = categoryName.find(" ", currPosition)) != std::string::npos)
		{
			categoryName = categoryName.replace(currPosition, 1, " ");
			currPosition++;
		}
		pathToImage.append(categoryName + "/");
		std::filesystem::create_directory(pathToImage);
		pathToImage.append(pageNumber + "/");
		std::filesystem::create_directory(pathToImage);
		return pathToImage;
	}

	bool  WebParser::getProductsRefPriceCard(std::vector<std::string>& refs, std::vector<std::string>& prices,
		std::string webPage)
	{
		bool productsWasFound = false;
		std::string productCardBlock = getProductCardBlock(webPage);
		DataSelector dataSelector = DataSelector(parseRules);
		std::vector<std::string> productCards = dataSelector.selectDataFromHTML("productCard", productCardBlock);
		if (productCards.size() > 0)
		{
			for (const auto& productCard: productCards)
			{
				productsWasFound = true;
				std::vector<std::string> refsTmp = dataSelector.selectDataFromHTML("ref", productCard);
				if (refsTmp.size() != 1)
					;//ErrorHandler::setErrorMessage("Canno`t get product ref from card");
				std::vector<std::string> pricesTmp = dataSelector.selectDataFromHTML("price_catalog", productCard);
				if (parseRules.count("notInStock") != 0)
				{
					std::vector<std::string> notInStock = dataSelector.selectDataFromHTML("notInStock", productCard);
					if (notInStock.size() >= 1)
						continue;
				}
				refs.push_back(refsTmp[0]);
				if (pricesTmp.size() > 0)
					prices.push_back(pricesTmp.back());
				else
					;//ErrorHandler::setErrorMessage("Canno`t get product price from card");
			}
		}
		//backup code if no card found
		else
		{
			//ErrorHandler::setErrorMessage("Canno`t get product cards");
			refs = dataSelector.selectDataFromHTML("ref", webPage);
			prices = dataSelector.selectDataFromHTML("price_catalog", webPage);
			if (refs.size() > 0)
				productsWasFound = true;
		}
		for (auto& ref:refs)
			ref = domenName + ref;
		return productsWasFound;
	}

	std::string WebParser::getProductCardBlock(std::string webPage)
	{
		std::string productCardBlock = webPage;
		if (parseRules.count("productCardBlock") != 0)
		{
			DataSelector dataSelector = DataSelector(parseRules);
			std::vector<std::string> productCards = dataSelector.selectDataFromHTML("productCardBlock", webPage);
			if (productCards.size() != 0)
				productCardBlock = productCards.back();
			else
			{
				;//ErrorHandler::setErrorMessage("Canno`t get products card block");
			}
		}
		return productCardBlock;
	}

	std::vector<ProductData>  WebParser::getOldProducts(std::vector<std::string>& refs, std::vector<std::string> prices)
	{
		std::string error = "";
		int errorCode = 0;
		std::filesystem::path dataBasePath = std::filesystem::current_path();
		std::filesystem::path dataBaseName = std::filesystem::path(sqliteDataBaseFileName);
		dataBasePath /= dataBaseName;
		//SQLiteDataBase sqliteDataBase = SQLiteDataBase(dataBasePath.string().c_str(), errorCode, error);
		//need rewrite to not call always create
		//sqliteDataBase.createDataBase(dataBasePath.string().c_str());
		std::vector<ProductData> newProducts;
		for (size_t j = 0; j < refs.size(); j++)
		{
			ProductData oldProduct;// = sqliteDataBase.getProduct(refs[j]);
			if (j < prices.size() && atoi(oldProduct.price.c_str()) == atoi(prices[j].c_str()))
			{
				refs.erase(refs.begin() + j);
				prices.erase(prices.begin() + j);
				newProducts.push_back(oldProduct);
				//sqliteDataBase.updateDateUpdatedData(oldProduct);
				j--;
			}
		}
		return newProducts;
	}

	//std::vector<ProductData> WebParser::getImageForOldProduct(std::string dirImage, std::vector<ProductData>& oldProducts)
	//{
	//	std::string error = "";
	//	int errorCode = 0;
	//	std::filesystem::path dataBasePath = std::filesystem::current_path();
	//	std::filesystem::path dataBaseName = std::filesystem::path(sqliteDataBaseFileName);
	//	dataBasePath /= dataBaseName;
	//	SQLiteDataBase sqliteDataBase = SQLiteDataBase(dataBasePath.string().c_str(), errorCode, error);
	//	std::vector<ProductData> productWithoutImage;
	//
	//	for (int i = 0; i < oldProducts.size(); i++)
	//	{
	//		Image* image =
	//			sqliteDataBase.getImage(atoi(oldProducts[i].product_id.c_str()), (int)_site);
	//		if (image->size > 0)
	//		{
	//			filesystem::path pathToImageDir = dirImage;
	//			pathToImageDir /= (std::to_string(i + 101) + image->extension);
	//			oldProducts[i].pathToImage = std::filesystem::absolute(pathToImageDir).string();
	//			std::ofstream file(pathToImageDir.string(), std::ofstream::binary);
	//			if (!file)
	//				productWithoutImage.push_back(oldProducts[i]);
	//			else
	//			{
	//				file.write(image->buffer, image->size);
	//				file.close();
	//				//image->~Image();
	//				delete image;
	//			}
	//
	//		}
	//		else
	//			productWithoutImage.push_back(oldProducts[i]);
	//	}
	//	return productWithoutImage;
	//
	//}

	std::vector<ProductData> WebParser::findOldProductWithImage(std::vector<ProductData> oldProducts,
		std::vector<ProductData> oldProductsWitoutImage)
	{
		std::vector<ProductData> oldProductWithImage;
		for (size_t i = 0; i < oldProducts.size(); i++)
		{
			bool productHaveImage = true;
			for (size_t j = 0; j < oldProductsWitoutImage.size(); j++)
				if (oldProducts[i].product_id == oldProductsWitoutImage[j].product_id)
					productHaveImage = false;
			if (productHaveImage)
				oldProductWithImage.push_back(oldProducts[i]);
		}
		return oldProductWithImage;
	}

	std::vector<ProductData> WebParser::getNewProducts(std::vector<std::string> refs, WebWorker* webWorker, std::string pathToImage)
	{
		std::vector<ProductData> newProducts;
		for (size_t i = 0; i < refs.size(); i++)
		{
			std::filesystem::path pathToCurrentImage = std::filesystem::absolute(pathToImage);
			pathToCurrentImage.append(to_string(i));
			ProductData product = getProductData(webWorker, refs[i], pathToCurrentImage.string());
			if (!product.price.empty())
				newProducts.push_back(product);
		}
		return newProducts;
	}

	ProductData WebParser::getProductData(WebWorker* webWorker, std::string productUrl, std::string pathToImage)
	{
		MemoryStruct mem = webWorker->getWebPage(productUrl.c_str());
		DataSelector dataSelector = DataSelector(parseRules);
		ProductData product;
		if (mem.size == 0)
		{
			//ErrorHandler::setErrorMessage("Canno`t get product page");
			return product;
		}
		std::vector<std::string>productIsNotAvailable = dataSelector.selectDataFromHTML("productIsNotAvailable", std::string(mem.memory));
		if (productIsNotAvailable.size() > 0)
			return product;
		product.name = dataSelector.selectDataFromHTML("name", std::string(mem.memory)).back();
		std::vector<std::string> strPrices = dataSelector.selectDataFromHTML("price", std::string(mem.memory));
		if (strPrices.size() == 0)
		{
			//ErrorHandler::setErrorMessage("Canno`t get product price");
			return product;
		}
		std::string strPrice = strPrices.back();
		vector<std::string> protuctIds = dataSelector.selectDataFromHTML("productID", std::string(mem.memory));
		if (protuctIds.size() == 0)
		{
			//ErrorHandler::setErrorMessage("Canno`t get product id");
			return product;
		}
		product.product_id = protuctIds.back();
		vector<string> manufacturers = dataSelector.selectDataFromHTML("manufacturer", std::string(mem.memory));
		if (!empty(manufacturers))
			product.manufacturer = manufacturers.back();
		std::vector<std::string> images = dataSelector.selectDataFromHTML("image", std::string(mem.memory));
		if (images.size() == 0)
		{
			//ErrorHandler::setErrorMessage("Canno`t get product image url");
			return product;
		}
		std::string image = images.back();
		std::string imageURL = domenName + image;
		product.image_url = imageURL;
		size_t spaceProsition;
		while ((spaceProsition = strPrice.find(" ")) != std::string::npos)
			strPrice = strPrice.erase(spaceProsition, 1);
		product.price = to_string((int)ceil(stoi(strPrice)));// +stoi(strPrice) * _margin / 100));
		//downloadImageProduct(webWorker, product, pathToImage);
		//size_t lastDot = image.find_last_of('.');
		//std::string imageExtension = image.substr(lastDot, image.size() - (image.size() - lastDot));
		//pathToImage += imageExtension;
		//product.pathToImage = pathToImage;
		//webWorker->download_file(imageURL.c_str(), pathToImage.c_str());

		product.url = productUrl;
		std::vector<std::string> descriptions = dataSelector.selectDataFromHTML("description", std::string(mem.memory));
		for (const auto& description:descriptions)
		{
			product.description += description;
		}
		product.description = dataSelector.removeHTMLTags(product.description);
		product.description = dataSelector.removeDoubleSpacesAndLF(product.description);
		product.description = dataSelector.removeDublicateRow(product.description);
		return product;
	}

	void WebParser::downloadAllImages(WebWorker* webWorker, std::vector<std::string>& refs, std::vector<ProductData>& products,
		std::filesystem::path pathToImageFolder)
	{
		size_t imageIndex = 0;
		size_t imageName = 0;
		for (imageIndex; imageIndex < products.size(); imageIndex++, imageName++)
		{
			std::filesystem::path pathToCurrentImage = std::filesystem::absolute(pathToImageFolder);
			pathToCurrentImage.append(to_string(imageName));
			bool imageWasDownloaded = downloadImageProduct(webWorker, products[imageIndex], pathToCurrentImage.string());
			if (imageWasDownloaded == false)
			{
				//try update product image data
				ProductData product = getProductData(webWorker, products[imageIndex].url, pathToCurrentImage.string());
				imageWasDownloaded = downloadImageProduct(webWorker, product, pathToCurrentImage.string());
				if (imageWasDownloaded == false)
				{
					//ErrorHandler::setErrorMessage("Can`t download image: " + products[imageIndex].url);
					products.erase(products.begin() + imageIndex);
					imageIndex--;
				}
				else
					refs.push_back(products[imageIndex].url);
			}
		}
	}

	bool  WebParser::downloadImageProduct(WebWorker* webWorker, ProductData& product, std::string pathToImage)
	{
		bool result = false;
		size_t lastDot = product.image_url.find_last_of('.');
		std::string imageExtension = product.image_url.substr(lastDot, product.image_url.size() - (product.image_url.size() - lastDot));
		pathToImage += imageExtension;
		product.pathToImage = pathToImage;
		result = webWorker->download_file(product.image_url.c_str(), pathToImage.c_str());
		return result;
	}

	std::vector<ProductData> WebParser::updateImagePathDataInNewProduct(std::vector<ProductData>newProducts,
		std::vector<ProductData>allProductWithoutImage)
	{
		for (size_t i = 0; i < newProducts.size(); i++)
			for (size_t j = 0; j < allProductWithoutImage.size(); j++)
				if (newProducts[i].product_id == allProductWithoutImage[j].product_id)
					newProducts[i].pathToImage = allProductWithoutImage[j].pathToImage;
		return newProducts;
	}

	//void  WebParser::addNewProducts(std::vector<std::string> refs, std::vector<ProductData> products)
	//{
	//	std::string error = "";
	//	int errorCode = 0;
	//	std::filesystem::path dataBasePath = std::filesystem::current_path();
	//	std::filesystem::path dataBaseName = std::filesystem::path(sqliteDataBaseFileName);
	//	dataBasePath /= dataBaseName;
	//	SQLiteDataBase sqliteDataBase = SQLiteDataBase(dataBasePath.string().c_str(), errorCode, error);
	//
	//	for (size_t i = 0; i < refs.size(); i++)
	//	{
	//		for (size_t j = 0; j < products.size(); j++)
	//		{
	//			if (refs[i] == products[j].url)
	//			{
	//				//if (!sqliteDataBase.productExist(refs[i]))
	//				sqliteDataBase.addProduct(products[j], (int)_site);
	//				break;
	//			}
	//		}
	//	}
	//}

	void WebParser::cleanUpTMPFolder()
	{
		filesystem::remove_all(tmpFolder);
	}

}
