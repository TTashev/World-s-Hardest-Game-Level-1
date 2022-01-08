#include <iostream>
#include <thread>
#include <vector>
#include <mutex>
#include <unistd.h>

#include "Bots.h"

#ifdef _WIN32
#include <windows.h>
#endif

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

std::string visited =
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
#ifdef _WIN32
	HANDLE hOut;
	// set x and y to 0 (upper left corner)
	COORD Position = {0, 0};

	hOut = GetStdHandle(STD_OUTPUT_HANDLE);

	SetConsoleCursorPosition(hOut, Position);
#endif

#ifdef linux
	std::cout << "\033[H\033[2J\033[3J";
#endif
}

void moveEnemy(int startPos, int endPos)
{
    std::chrono::time_point<std::chrono::system_clock> t = std::chrono::system_clock::now();
    while (true)
    {
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
    }
}

void findExit(int prevMoveIndex, int moveIndex)
{
    if(level[moveIndex] == '#' || level[moveIndex] == 'e' || level[moveIndex] == 'O' || visited[moveIndex] == 's')
    {
        return;
    }

    std::chrono::time_point<std::chrono::system_clock> t = std::chrono::system_clock::now();

    level[prevMoveIndex] = '.';
    level[moveIndex] = 's';
    visited[moveIndex] = 's';

    t += std::chrono::milliseconds(33);
    std::this_thread::sleep_until(t);

    findExit(moveIndex, moveIndex + 1);
    findExit(moveIndex, moveIndex + rowSize);
    findExit(moveIndex, moveIndex - 1);
    findExit(moveIndex, moveIndex - rowSize);
}

void run()
{
    // spawn enemy movements on different threads
    std::thread simulateEnemy1 ( moveEnemy, 159, 182 );
    std::thread simulateEnemy2 ( moveEnemy, 208, 231 );
    std::thread simulateEnemy3 ( moveEnemy, 257, 280 );
    std::thread simulateEnemy4 ( moveEnemy, 306, 329 );
    std::thread simulateEnemy5 ( moveEnemy, 355, 378 );

    //std::thread simulatePlayer (findExit, playerInitialPos, playerInitialPos + (rand() % 4 + 1));
    std::vector<Bot> bots;
    std::thread t[10];
    for(int bot = 0; bot < 10; bot++)
    {
        bots.push_back(Bot());
    }

    while(true)
    {
        std::chrono::time_point<std::chrono::system_clock> t = std::chrono::system_clock::now();
        // update the screen
        std::cout << level << std::endl;
        clearScreen();

        for(int i = 0; i < 10; i++)
        {
            // generate player moves
            int generateMove = rand() % 4 + 1;
            // move the player position
            bots[i].movePlayer(level, generateMove);
        }

        t += std::chrono::milliseconds(33);
        std::this_thread::sleep_until(t);
    }
}
