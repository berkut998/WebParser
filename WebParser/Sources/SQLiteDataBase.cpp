#include "SQLiteDataBase.h"
namespace WP
{
    SQLiteDataBase::SQLiteDataBase(const char* pathToDataBase, int& errorCode, std::string& error)
    {
        errorCode = sqlite3_open(pathToDataBase, &db);
        if (errorCode != SQLITE_OK)
            error = sqlite3_errmsg(db);
    }

    SQLiteDataBase::~SQLiteDataBase()
    {
        sqlite3_close(db);
    }

    bool  SQLiteDataBase::dataBaseFileExist(const char* pathToDataBase)
    {
        struct stat buffer;
        int fileExist = stat(pathToDataBase, &buffer);
        if (fileExist != 0)
            return false;
        return true;
    }

    bool SQLiteDataBase::createProductTable()
    {
        /*
       CREATE TABLE IF NOT EXISTS products (
       productId INTEGER PRIMARY KEY,
       productIdOnSite INTEGER UNIQUE,
       siteId INTEGER UNIQUE,
       name TEXT,
       manufacturer TEXT,
       price REAL,
       description TEXT,
       category INTEGER,
       url TEXT,
       image_url TEXT
       );

       CREATE [UNIQUE] INDEX proiductIdAndSiteId
       ON products(productIdOnSite,siteId);
       */
       //sqlite3_open(pathToDataBase, &db);
        bool tableWasCreated = true;
        int sqlResultCode = sqlite3_prepare_v2(db, R"(CREATE TABLE IF NOT EXISTS products (
        productId INTEGER PRIMARY KEY,
        productIdOnSite INTEGER,
        siteId INTEGER,
        name TEXT,
        manufacturer TEXT,
        price REAL,
        description TEXT,
        category INTEGER,
        url TEXT,
        image_url TEXT,
        date_updated DATE );)", -1, &queryResult, NULL);
        if (sqlResultCode != SQLITE_OK)
            tableWasCreated = false;
        sqlResultCode = sqlite3_step(queryResult);
        if (sqlResultCode != SQLITE_DONE)
            tableWasCreated = false;
        sqlite3_finalize(queryResult);
        return tableWasCreated;
    }

    bool SQLiteDataBase::createProductToImageTable()
    {

        int sqlResultCode = sqlite3_prepare_v2(db, R"( CREATE TABLE IF NOT EXISTS productToImage (
        productId INTEGER PRIMARY KEY,
        image BLOB,
        fileExtension TEXT))", -1, &queryResult, NULL);
        if (sqlResultCode != SQLITE_OK) {
            sqlite3_finalize(queryResult);
            sqlite3_close(db);
            return false;
        }
        sqlResultCode = sqlite3_step(queryResult);
        if (sqlResultCode != SQLITE_DONE)
        {
            const char* error = sqlite3_errmsg(db);
            sqlite3_finalize(queryResult);
            return false;
        }
        sqlite3_finalize(queryResult);
        return true;
    }



    bool SQLiteDataBase::createIndexForProdutcTable()
    {
        bool indexWasCreated = true;
        int sqlResultCode = sqlite3_prepare_v2(db, R"(CREATE UNIQUE INDEX IF NOT EXISTS proiductIdAndSiteId
        ON products(productIdOnSite, siteId))", -1, &queryResult, NULL);
        if (sqlResultCode != SQLITE_OK)
            indexWasCreated = false;
        sqlResultCode = sqlite3_step(queryResult);
        if (sqlResultCode != SQLITE_DONE)
            indexWasCreated = false;
        sqlite3_finalize(queryResult);
        if (indexWasCreated == false)
            return indexWasCreated;
        sqlResultCode = sqlite3_prepare_v2(db, R"(CREATE UNIQUE INDEX IF NOT EXISTS url
        ON products(url))", -1, &queryResult, NULL);
        if (sqlResultCode != SQLITE_OK)
            indexWasCreated = false;
        sqlResultCode = sqlite3_step(queryResult);
        if (sqlResultCode != SQLITE_DONE)
            indexWasCreated = false;
        sqlite3_finalize(queryResult);
        return indexWasCreated;
    }

    bool SQLiteDataBase::createDataBase(const char* pathToDataBase)
    {
        bool result = true;
        result = createProductTable();
        if (result == false)
            return result;
        result = createProductToImageTable();
        if (result == false)
            return result;
        result = createIndexForProdutcTable();
        if (result == false)
            return result;

        return result;
    }

    bool SQLiteDataBase::addProduct(ProductData product, int siteId)
    {
        bool productAlreadyExist = productExist(product.url);
        bool wasAdded = false;
        int productId;
        if (!productAlreadyExist)
        {
            wasAdded = addProductData(product, siteId);
            productId = getProductId(stoi(product.product_id), siteId);
        }
        else
            updateProduct(product, siteId);
        if (wasAdded == false)
            return wasAdded;
        wasAdded = addImage(productId, product.pathToImage);
        //by logic need delete product or return other value
        return wasAdded;
    }
    bool SQLiteDataBase::addProductData(ProductData product, int siteId)
    {
        product.category = "1";
        int sqlResultCode = sqlite3_prepare_v2(db, R"(INSERT INTO products 
    (productIdOnSite,
    siteId,
    name,
    manufacturer,
    price,
    description,
    category,
    url,
    image_url,
    date_updated)
    VALUES (?1,?2,?3,?4,?5,?6,?7,?8,?9,date());)", -1, &queryResult, NULL);
        if (sqlResultCode != SQLITE_OK) {
            sqlite3_finalize(queryResult);
            return false;
        }
        sqlite3_bind_int64(queryResult, 1, atoi(product.product_id.c_str()));
        sqlite3_bind_int64(queryResult, 2, siteId);
        sqlite3_bind_text(queryResult, 3, product.name.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_text(queryResult, 4, product.manufacturer.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_double(queryResult, 5, atof(product.price.c_str()));
        sqlite3_bind_text(queryResult, 6, product.description.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_int64(queryResult, 7, atoi(product.category.c_str()));
        sqlite3_bind_text(queryResult, 8, product.url.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_text(queryResult, 9, product.image_url.c_str(), -1, SQLITE_STATIC);
        sqlResultCode = sqlite3_step(queryResult);
        sqlite3_finalize(queryResult);
        if (sqlResultCode != SQLITE_DONE)
            return false;
        return true;
    }

    bool SQLiteDataBase::updateProduct(ProductData product, int siteId)
    {
        product.category = "1";
        int sqlResultCode = sqlite3_prepare_v2(db, R"(UPDATE products SET
    productIdOnSite = ?1,
    siteId = ?2,
    name = ?3,
    manufacturer = ?4,
    price = ?5,
    description = ?6,
    category = ?7,
    url = ?8,
    image_url = ?9,
    date_updated = date() WHERE url = ?8;)", -1, &queryResult, NULL);
        if (sqlResultCode != SQLITE_OK) {
            sqlite3_finalize(queryResult);
            return false;
        }
        sqlite3_bind_int64(queryResult, 1, atoi(product.product_id.c_str()));
        sqlite3_bind_int64(queryResult, 2, siteId);
        sqlite3_bind_text(queryResult, 3, product.name.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_text(queryResult, 4, product.manufacturer.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_double(queryResult, 5, atof(product.price.c_str()));
        sqlite3_bind_text(queryResult, 6, product.description.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_int64(queryResult, 7, atoi(product.category.c_str()));
        sqlite3_bind_text(queryResult, 8, product.url.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_text(queryResult, 9, product.image_url.c_str(), -1, SQLITE_STATIC);
        sqlResultCode = sqlite3_step(queryResult);
        sqlite3_finalize(queryResult);
        if (sqlResultCode != SQLITE_DONE)
            return false;
        return true;
    }

    int SQLiteDataBase::getProductId(int productIdOnSite, int siteId)
    {
        int productId = 0;
        int sqlResultCode = sqlite3_prepare_v2(db, R"(SELECT productId FROM products 
    WHERE productIdOnSite = ?1 AND siteId = ?2;)", -1, &queryResult, NULL);
        if (sqlResultCode != SQLITE_OK) {
            sqlite3_finalize(queryResult);
            return productId;
        }
        sqlite3_bind_int(queryResult, 1, productIdOnSite);
        sqlite3_bind_int(queryResult, 2, siteId);

        sqlResultCode = sqlite3_step(queryResult);

        if (sqlResultCode == SQLITE_ROW)
        {
            productId = sqlite3_column_int(queryResult, 0);
        }
        sqlite3_finalize(queryResult);
        return productId;
    }

    //image not updated only 1 time setted when products first add to database
    bool SQLiteDataBase::addImage(int productId, std::string pathToImage)
    {
        size_t lastDot = pathToImage.find_last_of('.');
        if (lastDot == std::string::npos)
            return false;
        std::string imageExtension = pathToImage.substr(lastDot, pathToImage.size() - (pathToImage.size() - lastDot));
        std::ifstream file(pathToImage, std::ios::in | std::ios::binary);
        if (!file) {
            return false;
        }
        file.seekg(0, std::ifstream::end);
        std::streampos size = file.tellg();
        file.seekg(0);
        char* buffer = new char[size];
        file.read(buffer, size);
        file.close();

        int sqlResultCode = sqlite3_prepare_v2(db, R"(INSERT INTO productToImage 
    (productId,
    image,
    fileExtension)
    VALUES (?1,?2,?3);)", -1, &queryResult, NULL);
        if (sqlResultCode != SQLITE_OK) {
            sqlite3_finalize(queryResult);
            return false;
        }
        sqlite3_bind_int64(queryResult, 1, productId);
        sqlite3_bind_blob(queryResult, 2, buffer, size, SQLITE_STATIC);
        sqlite3_bind_text(queryResult, 3, imageExtension.c_str(), -1, SQLITE_STATIC);
        sqlResultCode = sqlite3_step(queryResult);
        sqlite3_finalize(queryResult);
        delete[] buffer;
        if (sqlResultCode != SQLITE_DONE)
            return false;
        return true;
    }

    Image* SQLiteDataBase::getImageBuffer(std::string pathToImage)
    {
        Image* image = new Image();
        image->buffer = nullptr;
        image->size = 0;
        std::ifstream file(pathToImage, std::ios::in | std::ios::binary);
        if (!file) {
            return image;
        }
        file.seekg(0, std::ifstream::end);
        image->size = file.tellg();
        file.seekg(0);
        image->buffer = new char[image->size];
        file.read(image->buffer, image->size);
        file.close();
        return image;
    }

    Image* SQLiteDataBase::getImage(int productIdOnSite, int siteId)
    {
        char* buffer = nullptr;
        Image* image = new Image();
        int productId = getProductId(productIdOnSite, siteId);
        ProductData product;
        int sqlResultCode = sqlite3_prepare_v2(db, R"(SELECT image,fileExtension FROM productToImage 
    WHERE productId = ?1;)", -1, &queryResult, NULL);
        if (sqlResultCode != SQLITE_OK) {
            sqlite3_finalize(queryResult);
            return image;
        }
        sqlite3_bind_int(queryResult, 1, productId);
        sqlResultCode = sqlite3_step(queryResult);
        if (sqlResultCode == SQLITE_ROW)
        {
            image->size = sqlite3_column_bytes(queryResult, 0);
            image->buffer = (char*)malloc(image->size);
            memcpy(image->buffer, sqlite3_column_blob(queryResult, 0), image->size);
            image->extension = convertUnsignedCharToString(sqlite3_column_text(queryResult, 1));
        }
        sqlite3_finalize(queryResult);
        return image;
    }

    ProductData SQLiteDataBase::getProduct(std::string  product_url)
    {
        ProductData product;
        int sqlResultCode = sqlite3_prepare_v2(db, R"(SELECT * FROM products 
    WHERE url = ?1;)", -1, &queryResult, NULL);
        if (sqlResultCode != SQLITE_OK) {
            sqlite3_finalize(queryResult);
            return product;
        }
        sqlite3_bind_text(queryResult, 1, product_url.c_str(), -1, SQLITE_STATIC);

        sqlResultCode = sqlite3_step(queryResult);

        if (sqlResultCode == SQLITE_ROW)
        {
            /*
           (productIdOnSite,
           siteId,
           name,
           manufacturer,
           price,
           description,
           category,
           url,
           image_url)
           */
            product.product_id = std::to_string(sqlite3_column_int(queryResult, 1));
            product.name = convertUnsignedCharToString(sqlite3_column_text(queryResult, 3));
            product.manufacturer = convertUnsignedCharToString(sqlite3_column_text(queryResult, 4));
            product.price = std::to_string(sqlite3_column_double(queryResult, 5));
            product.description = convertUnsignedCharToString(sqlite3_column_text(queryResult, 6));
            product.category = std::to_string(sqlite3_column_int(queryResult, 7));
            product.url = convertUnsignedCharToString(sqlite3_column_text(queryResult, 8));
            product.image_url = convertUnsignedCharToString(sqlite3_column_text(queryResult, 9));
        }
        sqlite3_finalize(queryResult);
        return product;

    }

    std::string SQLiteDataBase::convertUnsignedCharToString(const unsigned char* str)
    {
        if (str == nullptr)
            return "";
        std::basic_string<unsigned char> temp = str;
        std::string firstItem(temp.begin(), temp.end());
        return firstItem;
    }

    bool SQLiteDataBase::productExist(std::string  product_url)
    {
        bool exist = false;
        int sqlResultCode = sqlite3_prepare_v2(db, R"(SELECT COUNT(*) as countExist FROM products 
    WHERE url = ?1;)", -1, &queryResult, NULL);
        if (sqlResultCode != SQLITE_OK) {
            sqlite3_finalize(queryResult);
            return exist;
        }
        sqlite3_bind_text(queryResult, 1, product_url.c_str(), -1, SQLITE_STATIC);
        sqlResultCode = sqlite3_step(queryResult);
        if (sqlResultCode == SQLITE_ROW)
            if (sqlite3_column_int(queryResult, 0) > 0)
                exist = true;
        sqlite3_finalize(queryResult);
        return exist;
    }
}
