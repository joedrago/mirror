#include <stdio.h>
#include <ncurses.h>

#include "lib_reflection/Init.h"
#include "lib_boardgame/Board.h"

using namespace std;

// ---------------------------------------------------------------------------

enum
{
    COLOR_BORDER = 1,
    COLOR_SELECTED,

    COLOR_WATER,
    COLOR_FARMS,
    COLOR_FOREST,
    COLOR_SWAMP,
    COLOR_HILLS,
    COLOR_MOUNTAINS,

    COLOR_COUNT
};

enum
{
    ATTR_BORDER = COLOR_PAIR(COLOR_BORDER),
    ATTR_SELECTED = COLOR_PAIR(COLOR_SELECTED),

    ATTR_WATER = COLOR_PAIR(COLOR_WATER),
    ATTR_FARMS = COLOR_PAIR(COLOR_FARMS),
    ATTR_FOREST = COLOR_PAIR(COLOR_FOREST),
    ATTR_SWAMP = COLOR_PAIR(COLOR_SWAMP),
    ATTR_HILLS = COLOR_PAIR(COLOR_HILLS),
    ATTR_MOUNTAINS = COLOR_PAIR(COLOR_MOUNTAINS),

    ATTR_COUNT
};

void initcolors()
{
    init_pair(COLOR_BORDER, COLOR_WHITE, COLOR_BLACK);
    init_pair(COLOR_SELECTED, COLOR_WHITE, COLOR_BLACK);

    init_pair(COLOR_WATER, COLOR_BLUE, COLOR_BLACK);
    init_pair(COLOR_FARMS, COLOR_YELLOW, COLOR_BLACK);
    init_pair(COLOR_FOREST, COLOR_GREEN, COLOR_BLACK);
    init_pair(COLOR_SWAMP, COLOR_MAGENTA, COLOR_BLACK);
    init_pair(COLOR_HILLS, COLOR_CYAN, COLOR_BLACK);
    init_pair(COLOR_MOUNTAINS, COLOR_RED, COLOR_BLACK);
}

// ---------------------------------------------------------------------------

MM_EXPOSED class SmallWorld : public Board
{
public:
    bool OnDrawSpace(Space *space, int boardx, int boardy);
    bool OnDrawBorder(int boardx, int boardy);
};

bool SmallWorld::OnDrawSpace(Space *space, int boardx, int boardy)
{
    if(space == selected)
    {
        attrset(ATTR_SELECTED);
    }
    else
    {
        switch(space->name[0])
        {
            case 'W': attrset(ATTR_WATER); break;
            case 'F': attrset(ATTR_FARMS); break;
            case 'T': attrset(ATTR_FOREST); break;
            case 'S': attrset(ATTR_SWAMP); break;
            case 'H': attrset(ATTR_HILLS); break;
            case 'M': attrset(ATTR_MOUNTAINS); break;
        };
    }
    return true;
}

bool SmallWorld::OnDrawBorder(int boardx, int boardy)
{
    attrset(ATTR_BORDER);
    return true;
}


void init()
{
    Reflection::Init();
    Reflection::MirrorFactory::LoadPath("data/boards");
    MIRROR_FORCE_REFERENCE(lib_boardgame_board_h);
}

void initgfx()
{
    initscr();
    start_color();
    initcolors();
}

void shutdown()
{
    endwin();
}

void draw(Board *board, int x, int y)
{
    board->draw(x, y);
    refresh();
}

void herp()
{
    getch();
}

void messWithSpaces(SmallWorld *sw)
{
    vector<Space*>::iterator spaceIt  = sw->spaces.begin();
    vector<Space*>::iterator spaceEnd = sw->spaces.end();


    for(; spaceIt != spaceEnd; spaceIt++)
    {
        Space *space = *spaceIt;

        char r = 'o';
        switch(space->name[0])
        {
            case 'W': r = '~'; break;
            case 'F': r = '#'; break;
            case 'T': r = 'T'; break;
            case 'S': r = '/'; break;
            case 'H': r = 'r'; break;
            case 'M': r = '^'; break;
        }

        vector<string> &art = space->art;
        for(unsigned int i=0; i<art.size(); i++)
        {
            string &s = art[i];
            for(unsigned int j=0; j<s.length(); j++)
            {
                if(s[j] != ' ')
                    s[j] = r;
            }
        }
    }
}

// ---------------------------------------------------------------------------

int main(int argc, char **argv)
{
    init();

    SmallWorld *sw = GET_NAMED_OBJECT(SmallWorld, "SWB2P");
    if(!sw)
    {
        printf("Failed to load board SWB2P\n");
        exit(-1);
    }

    messWithSpaces(sw);

    initgfx();
    draw(sw, 1, 1);
    herp();
    shutdown();

    return 0;
}

#include "smallworld/Main.cpp.mm.cpp"
