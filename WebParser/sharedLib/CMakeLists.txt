


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

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET WebParserSharedLib PROPERTY CXX_STANDARD 20)
endif()

# TODO: Add tests and install targets if needed.
