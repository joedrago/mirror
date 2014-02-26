#include "Board.h"

using namespace std;

Board::Board()
: selected(NULL)
{
}

Board::~Board()
{
}

void Board::OnPopulate()
{
    if(spaces.size())
    {
        selected = spaces[0];
    }
}

bool Board::OnDrawSpace(Space *space, int boardx, int boardy)
{
    return true;
}

bool Board::OnDrawBorder(int boardx, int boardy)
{
    return true;
}

inline std::string trim(const std::string& Src, const std::string& c = " ")
{
    int p2 = Src.find_last_not_of(c);
    if (p2 == std::string::npos) return std::string();
    int p1 = Src.find_first_not_of(c);
    if (p1 == std::string::npos) p1 = 0;
    return Src.substr(p1, (p2-p1)+1);
}

void Board::draw(int x, int y)
{
    vector<Space*>::iterator it  = spaces.begin();
    vector<Space*>::iterator end = spaces.end();
    for(; it != end; it++)
    {
        Space *space = *it;
        vector<string> *pArt = &space->art;
        if(!pArt->size())
            pArt = &baseart;

    if(OnDrawSpace(space, x, y))
    {
        unsigned int lineCount = pArt->size();
        for(unsigned int i=0; i<lineCount; i++)
        {
            string s = pArt->at(i);
            int leftoffset = 0;
            const char *c = s.c_str();
            while(*c == ' ')
            {
                leftoffset++;
                c++;
            }
            s = trim(s);
            mvprintw(i+y+space->pos.y, leftoffset+x+space->pos.x, s.c_str());
        }
    }
    }

    if(OnDrawBorder(x,y))
    {
        // draw border
        mvhline(y, x + 1, '-', width - 1);
        mvhline(y + height, x + 1, '-', width - 1);
        mvvline(y + 1, x, '|', height - 1);
        mvvline(y + 1, x + width, '|', height - 1);
        mvaddch(y, x, '/');// tl
        mvaddch(y, x + width, '\\'); // tr
        mvaddch(y + height, x, '\\'); // bl
        mvaddch(y + height, x + width, '/'); // br
    }
}

#include "lib_boardgame/Board.h.mm.cpp"

