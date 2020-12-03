#ifndef BOT_H_INCLUDED
#define BOT_H_INCLUDED

class Bot
{
public:
    Bot() : isDead(false), startLocation(249), movesLimit(50) {};
    ~Bot() {};

private:
    bool isDead;
    int startLocation;
    int movesLimit;
    int moves[50];
};

#endif // BOT_H_INCLUDED
