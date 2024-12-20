
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


add_executable (sqliteDataBaseTest "sqliteDataBaseTest.cpp" "${WEBPARSER_SOURCE_DIR}/SQLiteDataBase.cpp" "${SQLite3_SOURCE_Amalgamation}" )
target_link_libraries(sqliteDataBaseTest PRIVATE Boost::unit_test_framework)
target_include_directories(sqliteDataBaseTest PRIVATE "${HEADERS_DIR}" ${SQLite3_INCLUDE_DIRS_Amalgamation})
target_compile_features(sqliteDataBaseTest PRIVATE cxx_std_17)
add_test(NAME sqliteDataBaseTest COMMAND sqliteDataBaseTest)
if (WIN32)
        add_custom_command (
            TARGET "sqliteDataBaseTest" POST_BUILD
            COMMAND "${CMAKE_COMMAND}" -E copy -t "$<TARGET_FILE_DIR:sqliteDataBaseTest>"
                    "$<TARGET_RUNTIME_DLLS:sqliteDataBaseTest>" USES_TERMINAL COMMAND_EXPAND_LISTS
        )
endif ()

include (CTest)
enable_testing()





