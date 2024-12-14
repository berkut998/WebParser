# CMakeList.txt : CMake project for WebParser, include source and define
# project specific logic here.
#


add_library(WebParserLib STATIC ${SOURCE_FILES})

# Create a shared library
#add_library(MySharedLib SHARED ${SOURCE_FILES})
# Add source to this project's executable.
#add_executable (WebParser "WebParser.cpp" ${SOURCE_FILES})

target_include_directories(WebParserLib PRIVATE 
"${HEADERS_DIR}"
)

target_link_libraries(WebParserLib PRIVATE 
	CURL::libcurl
	Boost::headers 
	Boost::uuid 
	Boost::beast 
	Boost::property_tree 
	Boost::signals2 
	TBB::tbb 
)

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET WebParserLib PROPERTY CXX_STANDARD 20)
endif()

# TODO: Add tests and install targets if needed.
