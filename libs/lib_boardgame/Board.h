#ifndef BOARDGAME_BOARD_H
#define BOARDGAME_BOARD_H

#include "lib_core/StdTypes.h"
#include "lib_reflection/Mirror.h"

#include <ncurses.h>

MM_EXPOSED class Position
{
public:
    int x;
    int y;
};

MM_EXPOSED class Space
{
public:
    char *name;
    Position pos;
    StringVector art;
};

MM_EXPOSED class Board : public Reflection::Aware
{
public:
    Board();
    ~Board();

    virtual void OnPopulate();
    virtual bool OnDrawSpace(Space *space, int boardx, int boardy);
    virtual bool OnDrawBorder(int boardx, int boardy);

    int width;
    int height;

    void draw(int x, int y);

    StringVector baseart;
    std::vector<Space*> spaces;

    MM_IGNORE_START

    Space* selected;

    MM_IGNORE_END
};

#include "lib_boardgame/Board.h.mm.h"

#endif
