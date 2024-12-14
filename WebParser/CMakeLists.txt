# CMakeList.txt : CMake project for WebParser, include source and define
# project specific logic here.
#


include(FetchContent)

FetchContent_Declare(
  curl
  URL https://curl.se/download/curl-8.5.0.tar.gz
  DOWNLOAD_EXTRACT_TIMESTAMP true
  OVERRIDE_FIND_PACKAGE # this one
)

FetchContent_Declare(
  boost
  URL https://github.com/boostorg/boost/releases/download/boost-1.85.0/boost-1.85.0-cmake.zip
  DOWNLOAD_EXTRACT_TIMESTAMP true
  OVERRIDE_FIND_PACKAGE # this one
)

FetchContent_Declare(
  tbb
  URL https://github.com/uxlfoundation/oneTBB/releases/download/v2022.0.0/oneapi-tbb-2022.0.0-win.zip
  DOWNLOAD_EXTRACT_TIMESTAMP true
  OVERRIDE_FIND_PACKAGE # this one
)
list(APPEND CMAKE_MODULE_PATH "${FETCHCONTENT_BASE_DIR}/tbb-src/lib/cmake/tbb")
include(TBBConfig)

find_package(curl)
find_package(boost REQUIRED  uuid signals2 beast property_tree)
find_package(tbb REQUIRED tbb)


set (SOURCE_FILES  
"Sources/WebParser.cpp" 
"Sources/WebWorker.cpp"  
"Sources/SitePatternsLoader.cpp" 
"Sources/DataSelector.cpp"
)

# Create a static library
add_library(Adddd SHARED ${SOURCE_FILES})

# Create a shared library
#add_library(MySharedLib SHARED ${SOURCE_FILES})
# Add source to this project's executable.
#add_executable (WebParser "WebParser.cpp" ${SOURCE_FILES})

target_include_directories(Adddd PUBLIC 
"${CMAKE_CURRENT_SOURCE_DIR}/Headers/"
)

target_link_libraries(Adddd PRIVATE 
	CURL::libcurl
	Boost::headers 
	Boost::uuid 
	Boost::beast 
	Boost::property_tree 
	Boost::signals2 
	TBB::tbb 
)

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET Adddd PROPERTY CXX_STANDARD 20)
endif()

# TODO: Add tests and install targets if needed.
