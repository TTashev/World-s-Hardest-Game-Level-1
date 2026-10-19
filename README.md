level 1 of World's hardest game using the console.

https://sites.google.com/site/tyronesgameshack/worlds-hardest-game

## Reinforcement learning player

The player is controlled by a tabular Q-learning agent (`QLearning.h` / `QLearning.cpp`) that learns to reach the exit `e` while dodging the enemies `O`.

- **Environment**: a deterministic simulation of the level. Enemies bounce along their lane one cell per step, so their positions depend only on the step counter.
- **State**: player cell + enemy phase (step modulo the enemy cycle length).
- **Actions**: stay, left, right, down, up.
- **Rewards**: `-1` per step, `-100` when hit by an enemy (episode ends), `+100` on reaching the exit, plus potential-based shaping from the BFS distance to the exit.

On start the agent trains for 20000 episodes (about a second), prints its progress, then replays the learned path in the console at 30 fps.

Build on Linux:

```
g++ -std=c++17 -O2 -pthread HardestGame_level1.cpp Bots.cpp QLearning.cpp -o HardestGame
./HardestGame
```
