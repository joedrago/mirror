if(MSVC)
	set(CMAKE_C_FLAGS /wd4996)
endif()

project(test_reflection)
include_directories(${cjson_SOURCE_DIR})
add_executable(test_reflection 
    test_reflection.cpp
)

target_link_libraries(test_reflection lib_reflection cjson)

