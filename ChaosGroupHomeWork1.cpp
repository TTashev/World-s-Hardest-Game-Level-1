#include <iostream>
#include <windows.h>
#include <thread>
#include <unistd.h>
#include "HardestGame.h"

static const int rowSize = 49;
static const int playerInitialPos = 249;

std::string level =
".................................###############\n"
"#########........................#.............#\n"
"#.......#..#######################..####.......#\n"
"#.......#..#O.......................#..#.......#\n"
"#.......#..#.......................O#..#.......#\n"
"#...s...#..#O.......................#..#...e...#\n"
"#.......#..#.......................O#..#.......#\n"
"#.......#..#O.......................#..#.......#\n"
"#.......####..#######################..#.......#\n"
"#.............#........................#.......#\n"
"###############........................#########\n";

void run();

int main()
{
    run();
    return 0;
}

void clearScreen()
{
	HANDLE hOut;
	// set x and y to 0 (upper left corner)
	COORD Position = {0, 0};

	hOut = GetStdHandle(STD_OUTPUT_HANDLE);

	SetConsoleCursorPosition(hOut, Position);
}

void moveEnemy(int startP, int endP)
{
start:
    if(level[endP] == 'O')
    {
        for(int j = endP - 1; j >= startP; j--)
        {
            level[j] = 'O';
            level[j + 1] = '.';

            usleep(20000);
        }
    }
    else if(level[startP] == 'O')
    {
        for(int i = startP + 1; i < endP + 1; i++)
        {
            level[i] = 'O';
            level[i - 1] = '.';

            usleep(20000);
        }
    }
    goto start;
}

void movePlayer(int moveIndex)
{
    static int playerStartPos = 249;

    // check for the next move if it is a wall
    if(moveIndex == 1 && level[playerStartPos + 1] == '#') return;
    if(moveIndex == 2 && level[playerStartPos - 1] == '#') return;
    if(moveIndex == 3 && level[playerStartPos + rowSize] == '#') return;
    if(moveIndex == 4 && level[playerStartPos - rowSize] == '#') return;

    // reset player if hit by an enemy
    if(moveIndex == 1 && level[playerStartPos + 1] == 'O') {playerStartPos = playerInitialPos; return;}
    if(moveIndex == 2 && level[playerStartPos - 1] == 'O') {playerStartPos = playerInitialPos; return;}
    if(moveIndex == 3 && level[playerStartPos + rowSize] == 'O') {playerStartPos = playerInitialPos; return;}
    if(moveIndex == 4 && level[playerStartPos - rowSize] == 'O') {playerStartPos = playerInitialPos; return;}

    // reset the current player position to empty before we move him to the next position
    level[playerStartPos] = '.';
    if(moveIndex == 1)
    {
        // right movement
        playerStartPos++;
    }
    if(moveIndex == 2)
    {
        // left movement
        playerStartPos -= 1;
    }
    if(moveIndex == 3)
    {
        // down movement
        playerStartPos += rowSize;
    }
    if(moveIndex == 4)
    {
        // up movement
        playerStartPos -= rowSize;
    }

    level[playerStartPos] = 's';
    usleep(50000);
}

void run()
{
    // spawn enemy movements on different threads
    std::thread simulateEnemy1 ( moveEnemy, 159, 182 );
    std::thread simulateEnemy2 ( moveEnemy, 208, 231 );
    std::thread simulateEnemy3 ( moveEnemy, 257, 280 );
    std::thread simulateEnemy4 ( moveEnemy, 306, 329 );
    std::thread simulateEnemy5 ( moveEnemy, 355, 378 );

    while(true)
    {
        // update the screen
        std::cout << level << std::endl;
        clearScreen();

        // generate player moves
        int generateMove = rand() % 4 + 1;
        movePlayer(generateMove);
    }
}
