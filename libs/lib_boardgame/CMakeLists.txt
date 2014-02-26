
if(MSVC)
    include_directories(${pdcurses_SOURCE_DIR})
endif()

project(lib_boardgame)
set(BOARDGAME_SRCS
	Board.cpp
	Board.h
)

automm(lib_boardgame ${BOARDGAME_SRCS})
add_library(lib_boardgame ${BOARDGAME_SRCS})
