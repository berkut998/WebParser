


add_library(WebParserSharedLib SHARED ${SOURCE_FILES})



target_include_directories(WebParserSharedLib PRIVATE 
"${HEADERS_DIR}"
)

target_link_libraries(WebParserSharedLib PRIVATE 
	CURL::libcurl
	Boost::headers 
	Boost::uuid 
	Boost::beast 
	Boost::property_tree 
	Boost::signals2 
	TBB::tbb 
)

target_compile_features(WebParserSharedLib PRIVATE cxx_std_17)


# TODO: Add tests and install targets if needed.
