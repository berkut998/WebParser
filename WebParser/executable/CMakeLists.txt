﻿# CMakeList.txt : CMake project for WebParser, include source and define
# project specific logic here.
#


message(${SOURCE_FILES})


# Create a shared library
#add_library(MySharedLib SHARED ${SOURCE_FILES})
# Add source to this project's executable.
#add_executable (WebParser "WebParser.cpp" ${SOURCE_FILES})
add_executable(WebParser   "${CMAKE_CURRENT_SOURCE_DIR}/main.cpp" "${SOURCE_FILES}" )

target_include_directories(WebParser PUBLIC 
"${HEADERS_DIR}"
)



target_link_libraries(WebParser PUBLIC 
	CURL::libcurl
	Boost::headers 
	Boost::uuid 
	Boost::beast 
	Boost::property_tree 
	Boost::signals2 
	TBB::tbb 
)

target_compile_features(WebParser PRIVATE cxx_std_17)

if (WIN32)
        add_custom_command (
            TARGET "WebParser" POST_BUILD
            COMMAND "${CMAKE_COMMAND}" -E copy -t "$<TARGET_FILE_DIR:WebParser>"
                    "$<TARGET_RUNTIME_DLLS:WebParser>" USES_TERMINAL COMMAND_EXPAND_LISTS
        )
endif ()

# TODO: Add tests and install targets if needed.
