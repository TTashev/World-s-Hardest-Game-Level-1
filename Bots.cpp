#include "Bots.h"

#include <thread>

void Bot::movePlayer(std::string& level, int moveIndex)
{
    // check for the next move if it is a wall
    if(moveIndex == E_DIRECTIONS::LEFT && level[playerCurrentPos - 1] == '#') return;
    if(moveIndex == E_DIRECTIONS::RIGHT && level[playerCurrentPos + 1] == '#') return;
    if(moveIndex == E_DIRECTIONS::DOWN && level[playerCurrentPos + rowSize] == '#') return;
    if(moveIndex == E_DIRECTIONS::UP && level[playerCurrentPos - rowSize] == '#') return;

    // reset player if hit by an enemy
    if(moveIndex == E_DIRECTIONS::LEFT && level[playerCurrentPos - 1] == 'O') {playerCurrentPos = playerInitialPos; return;}
    if(moveIndex == E_DIRECTIONS::RIGHT && level[playerCurrentPos + 1] == 'O') {playerCurrentPos = playerInitialPos; return;}
    if(moveIndex == E_DIRECTIONS::DOWN && level[playerCurrentPos + rowSize] == 'O') {playerCurrentPos = playerInitialPos; return;}
    if(moveIndex == E_DIRECTIONS::UP && level[playerCurrentPos - rowSize] == 'O') {playerCurrentPos = playerInitialPos; return;}

    // reset the current player position before we move him to the next position
    level[playerCurrentPos] = '.';
    if(moveIndex == E_DIRECTIONS::LEFT)
    {
        // left movement
        playerCurrentPos--;
    }
    if(moveIndex == E_DIRECTIONS::RIGHT)
    {
        // right movement
        playerCurrentPos++;
    }
    if(moveIndex == E_DIRECTIONS::DOWN)
    {
        // down movement
        playerCurrentPos += rowSize;
    }
    if(moveIndex == E_DIRECTIONS::UP)
    {
        // up movement
        playerCurrentPos -= rowSize;
    }

    level[playerCurrentPos] = 's';
}
