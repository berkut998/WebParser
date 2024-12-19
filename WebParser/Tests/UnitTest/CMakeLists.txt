
project(UINT_TEST)
#DataSelector tests
add_executable (dataSelectorTest "dataSelectorTest.cpp" "${WEBPARSER_SOURCE_DIR}/DataSelector.cpp")
target_link_libraries(dataSelectorTest PRIVATE Boost::unit_test_framework)
target_include_directories(dataSelectorTest PRIVATE "${HEADERS_DIR}")
target_compile_features(dataSelectorTest PRIVATE cxx_std_17)
add_test(NAME dataSelectorTest COMMAND dataSelectorTest)

if (WIN32)
        add_custom_command (
            TARGET "dataSelectorTest" POST_BUILD
            COMMAND "${CMAKE_COMMAND}" -E copy -t "$<TARGET_FILE_DIR:dataSelectorTest>"
                    "$<TARGET_RUNTIME_DLLS:dataSelectorTest>" USES_TERMINAL COMMAND_EXPAND_LISTS
        )
endif ()

#DataSelectorRule tests
add_executable (dataSelectorRuleTest "dataSelectorRulesTest.cpp"   "${WEBPARSER_SOURCE_DIR}/DataSelectorRules.cpp")
target_link_libraries(dataSelectorRuleTest PRIVATE Boost::unit_test_framework)
target_include_directories(dataSelectorRuleTest PRIVATE "${HEADERS_DIR}")
target_compile_features(dataSelectorRuleTest PRIVATE cxx_std_17)
add_test(NAME dataSelectorRuleTest COMMAND dataSelectorRuleTest)

include (CTest)
enable_testing()





