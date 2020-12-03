#ifndef HARDESTGAME_H_INCLUDED
#define HARDESTGAME_H_INCLUDED

#include <vector>

const int PLAYER_SPEED = 10;
const int ENEMY_SPEED = 15;

struct Point {
	int x;
	int y;
};

struct Box {
	Point topLeft;
	Point bottomRight;
};

struct EnemyPath {
	Point start;
	Point end;
};

struct LevelDescription {
	std::vector<Box> playableArea; ///< The union of these boxes makes the level's area
	Box start; ///< The starting area, the player starts at the center of this box
	Box end; ///< The player needs to touch this box to finish successfully
	std::vector<EnemyPath> enemyPaths; ///< All enemies in the level, if a player touches them he looses
};

enum Direction {
	directionNone = 0,
	directionUp = 1 << 0,
	directionDown = 1 << 1,
	directionLeft = 1 << 2,
	directionRight = 1 << 3,
};

/// Based on a level description generate what commands would a player give to complete the level
/// Direction may be combined, like directionDown | directionRight to move diagonally to down-right
std::vector<Direction> generateBestPath(const LevelDescription &level);

#endif // HARDESTGAME_H_INCLUDED
