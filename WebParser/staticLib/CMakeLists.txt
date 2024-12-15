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

target_compile_features(WebParserLib PRIVATE cxx_std_17)


# TODO: Add tests and install targets if needed.
