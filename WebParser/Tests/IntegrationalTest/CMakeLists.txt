
project(INTEGRATIONAL_TEST)



add_executable (webWorkerTest "webWorkerTest.cpp" "${WEBPARSER_SOURCE_DIR}/WebWorker.cpp")
target_link_libraries(webWorkerTest PRIVATE Boost::unit_test_framework Boost::beast CURL::libcurl )
target_include_directories(webWorkerTest PRIVATE "${HEADERS_DIR}")
target_compile_features(webWorkerTest PRIVATE cxx_std_17)
add_test(NAME webWorkerTest COMMAND webWorkerTest)

if (WIN32)
        add_custom_command (
            TARGET "webWorkerTest" POST_BUILD
            COMMAND "${CMAKE_COMMAND}" -E copy -t "$<TARGET_FILE_DIR:webWorkerTest>"
                    "$<TARGET_RUNTIME_DLLS:webWorkerTest>" USES_TERMINAL COMMAND_EXPAND_LISTS
        )
endif ()


add_executable (webParserTest "webParserTest.cpp" "${SOURCE_FILES}")
target_link_libraries(webParserTest PUBLIC 
	CURL::libcurl
	Boost::headers 
	Boost::uuid 
	Boost::beast 
	Boost::property_tree 
	Boost::signals2 
    Boost::unit_test_framework
	TBB::tbb 
)
target_include_directories(webParserTest PRIVATE "${HEADERS_DIR}")
target_compile_features(webParserTest PRIVATE cxx_std_17)
add_test(NAME webParserTest COMMAND webParserTest)

if (WIN32)
        add_custom_command (
            TARGET "webParserTest" POST_BUILD
            COMMAND "${CMAKE_COMMAND}" -E copy -t "$<TARGET_FILE_DIR:webParserTest>"
                    "$<TARGET_RUNTIME_DLLS:webParserTest>" USES_TERMINAL COMMAND_EXPAND_LISTS
        )
endif ()

include (CTest)
enable_testing()





