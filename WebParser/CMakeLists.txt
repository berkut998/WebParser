# CMakeList.txt : CMake project for WebParser, include source and define
# project specific logic here.
#


include(FetchContent)

#SSL Windows for curl
set(USE_WINDOWS_SSPI ON)
set(USE_SCHANNEL ON)

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
#useless
list(APPEND CMAKE_PREFIX_PATH  "${FETCHCONTENT_BASE_DIR}/curl-src/CMake")

include(TBBConfig)

find_package(curl)
find_package(boost REQUIRED  uuid signals2 beast property_tree unit_test_framework asio)
find_package(tbb REQUIRED tbb)

set(WEBPARSER_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/Sources")

set (SOURCE_FILES  
"${CMAKE_CURRENT_SOURCE_DIR}/Sources/WebParser.cpp" 
"${CMAKE_CURRENT_SOURCE_DIR}/Sources/WebWorker.cpp"  
"${CMAKE_CURRENT_SOURCE_DIR}/Sources/DataSelector.cpp"
)

set (HEADERS_DIR "${CMAKE_CURRENT_SOURCE_DIR}/Headers")

add_subdirectory("executable")
add_subdirectory("sharedLib")
add_subdirectory("staticLib")
add_subdirectory("Tests/UnitTest")
add_subdirectory("Tests/IntegrationalTest")


# TODO: Add tests and install targets if needed.
