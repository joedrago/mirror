
project(smallworld C)

# -------------------------------------------------------------------------------
# Source Listings

if(MSVC)
    include_directories(${pdcurses_SOURCE_DIR})
endif()

set(GAME_SRCS
    Main.cpp
)

set(GAME_LIBS
    lib_reflection
    lib_boardgame
)

automm(smallworld ${GAME_SRCS})

# -------------------------------------------------------------------------------

if(APPLE)
    set(GUI_TYPE MACOSX_BUNDLE)
endif()

add_executable(smallworld
    ${GUI_TYPE}
    ${GAME_SRCS}
)

target_link_libraries (smallworld ${GAME_LIBS})

if(MSVC)
    target_link_libraries(smallworld ${pdcurses_SOURCE_DIR}/pdcurses.lib)
else()
    target_link_libraries(smallworld ncurses)
endif()

