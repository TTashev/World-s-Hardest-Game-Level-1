#include <iostream>
#include <windows.h>
#include <thread>
#include <chrono>
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

void moveEnemy(int startPos, int endPos)
{
    std::chrono::time_point<std::chrono::system_clock> t = std::chrono::system_clock::now();
start:
    if(level[endPos] == 'O')
    {
        for(int j = endPos - 1; j >= startPos; j--)
        {
            level[j] = 'O';
            level[j + 1] = '.';

            t += std::chrono::milliseconds(33);
            std::this_thread::sleep_until(t);
        }
    }
    else if(level[startPos] == 'O')
    {
        for(int i = startPos + 1; i < endPos + 1; i++)
        {
            level[i] = 'O';
            level[i - 1] = '.';

            t += std::chrono::milliseconds(33);
            std::this_thread::sleep_until(t);
        }
    }
    goto start;
}

void movePlayer(int moveIndex)
{
    static int playerStartPos = 249;

    std::chrono::time_point<std::chrono::system_clock> t = std::chrono::system_clock::now();

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
        playerStartPos--;
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

    t += std::chrono::milliseconds(33);
    std::this_thread::sleep_until(t);
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
