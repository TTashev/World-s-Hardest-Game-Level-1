#ifndef BOT_H_INCLUDED
#define BOT_H_INCLUDED

#include <string>
#include <chrono>

enum E_DIRECTIONS
{
    LEFT = 1,
    RIGHT,
    DOWN,
    UP,
};

static const int rowSize = 49;
static const int playerInitialPos = 249;

class Bot
{
public:
    Bot() : isDead(false), playerCurrentPos(249), movesLimit(10) {};
    ~Bot() {};

    void movePlayer(std::string& level, int moveIndex);
private:
    bool isDead;
    int playerCurrentPos;
    int movesLimit;
    int moves[10];
};

#endif // BOT_H_INCLUDED
