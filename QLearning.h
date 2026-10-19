#ifndef QLEARNING_H_INCLUDED
#define QLEARNING_H_INCLUDED

#include <random>
#include <string>
#include <vector>

enum E_ACTIONS
{
    ACTION_STAY = 0,
    ACTION_LEFT,
    ACTION_RIGHT,
    ACTION_DOWN,
    ACTION_UP,
    ACTIONS_COUNT
};

/// Deterministic simulation of the level used to train the agent.
/// Enemies bounce along their lane one cell per step, so their positions are a
/// pure function of the step counter and the state can be (cell, enemy phase).
class Environment
{
public:
    struct StepResult
    {
        double reward;
        bool done;
        bool reachedExit;
        bool died;
    };

    explicit Environment(const std::string& levelLayout);

    void reset();
    StepResult step(int action);

    int stateIndex() const;
    int stateCount() const;
    int playerPos() const { return playerPos_; }
    int stepCount() const { return step_; }

    /// Level with walls, exit, enemies and the player at the current step
    std::string render() const;

private:
    struct Lane
    {
        int first;
        int length;
        bool startsAtFirst;
    };

    bool isWall(int pos) const;
    int move(int pos, int action) const;
    int enemyPos(const Lane& lane, int step) const;
    bool hitsEnemy(int oldPos, int newPos, int oldStep, int newStep) const;
    void computeDistances();

    std::string grid_;
    int rowSize_;
    int rows_;
    int startPos_;
    int exitPos_;
    int period_;
    std::vector<Lane> lanes_;
    std::vector<int> distToExit_;

    int playerPos_;
    int step_;
};

/// Tabular Q-learning agent with an epsilon-greedy exploration policy
class QLearningAgent
{
public:
    struct Params
    {
        int episodes = 20000;
        int maxStepsPerEpisode = 500;
        double alpha = 0.1;
        double gamma = 0.99;
        double epsilonStart = 1.0;
        double epsilonEnd = 0.01;
        unsigned seed = 42;
    };

    QLearningAgent(int stateCount, const Params& params);

    void train(Environment& env, bool verbose = true);
    int bestAction(int state) const;

    /// Greedy rollout of the learned policy; returns the chosen actions
    std::vector<int> greedyRollout(Environment& env, bool& reachedExit) const;

private:
    int chooseAction(int state, double epsilon);
    double& q(int state, int action) { return table_[state * ACTIONS_COUNT + action]; }
    double q(int state, int action) const { return table_[state * ACTIONS_COUNT + action]; }

    Params params_;
    std::vector<double> table_;
    std::mt19937 rng_;
};

#endif // QLEARNING_H_INCLUDED
