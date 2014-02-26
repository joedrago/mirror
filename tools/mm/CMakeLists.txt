# Magic Mirror

if(MSVC)
	set(CMAKE_C_FLAGS /wd4996)
endif()

project(mm)
add_executable(mm 
    mm.cpp
    tokens.cpp
    tokens.h
    MacroGenerator.cpp
    MacroGenerator.h
)

target_link_libraries(mm lib_reflection)
