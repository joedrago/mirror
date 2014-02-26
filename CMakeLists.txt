cmake_minimum_required(VERSION 2.6)

include("automm.cmake")

project(src)

set_property(GLOBAL PROPERTY AUTO_PROJECT_GROUPS 1)

if(UNIX)
    set(CMAKE_CXX_FLAGS "-g -Wno-invalid-offsetof")
    set(CMAKE_C_FLAGS "-g")
endif()

include_directories(${CMAKE_SOURCE_DIR}/libs)

if(APPLE)
    add_definitions(-DPLATFORM_OSX)
else()
    if(UNIX)
        add_definitions(-DPLATFORM_LINUX)
    endif()

    if(MSVC)
        add_definitions(-DPLATFORM_WIN32)
    endif()

endif()

add_subdirectory(libs)
add_subdirectory(games)
add_subdirectory(external)
add_subdirectory(tools)

