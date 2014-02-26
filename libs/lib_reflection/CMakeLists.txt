
project(lib_reflection)
include_directories(${cjson_SOURCE_DIR})
add_library(lib_reflection 
	Init.h
	Init.cpp
    Mirror.cpp
    Mirror.h
    MirrorFactory.cpp
    MirrorFactory.h
    MirrorMacros.h
)
target_link_libraries(lib_reflection lib_core cjson)
