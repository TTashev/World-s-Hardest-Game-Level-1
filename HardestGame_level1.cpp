#include <cstdlib>
#include <iostream>
#include <thread>
#include <vector>
#include <mutex>

#include "Bots.h"
#include "QLearning.h"

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
    // train a Q-learning agent on a deterministic simulation of the level
    Environment env(level);
    QLearningAgent::Params params;
    QLearningAgent agent(env.stateCount(), params);

    std::cout << "Training Q-learning agent..." << std::endl;
    agent.train(env);

    bool reachedExit = false;
    std::vector<int> actions = agent.greedyRollout(env, reachedExit);
    std::cout << (reachedExit ? "Learned a path to the exit in " : "Agent did not reach the exit, best attempt: ")
              << actions.size() << " steps" << std::endl;
    std::this_thread::sleep_for(std::chrono::seconds(2));

#ifdef _WIN32
    // clearScreen() only moves the cursor on Windows, so wipe the training log once
    system("cls");
#endif

    // replay the learned policy at 30 fps
    while(true)
    {
        env.reset();
        for(size_t i = 0; i <= actions.size(); i++)
        {
            std::chrono::time_point<std::chrono::system_clock> t = std::chrono::system_clock::now();

            clearScreen();
            std::cout << env.render() << "step " << env.stepCount() << "/" << actions.size() << std::endl;

            if(i < actions.size())
            {
                env.step(actions[i]);
            }

            t += std::chrono::milliseconds(33);
            std::this_thread::sleep_until(t);
        }
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
}
