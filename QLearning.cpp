#include "QLearning.h"

#include <algorithm>
#include <iostream>
#include <numeric>
#include <queue>

namespace
{
const double STEP_REWARD = -1.0;
const double DEATH_REWARD = -100.0;
const double EXIT_REWARD = 100.0;
const double SHAPING_SCALE = 1.0;
const int UNREACHABLE = -1;
}

Environment::Environment(const std::string& levelLayout)
    : rowSize_(static_cast<int>(levelLayout.find('\n')) + 1)
    , startPos_(-1)
    , exitPos_(-1)
    , period_(1)
    , playerPos_(0)
    , step_(0)
{
    grid_ = levelLayout;
    rows_ = static_cast<int>(grid_.size()) / rowSize_;

    for (int pos = 0; pos < static_cast<int>(grid_.size()); pos++)
    {
        if (grid_[pos] == 's') { startPos_ = pos; grid_[pos] = '.'; }
        if (grid_[pos] == 'e') { exitPos_ = pos; }
    }

    // every enemy bounces inside the open run of cells of its row it starts in
    for (int pos = 0; pos < static_cast<int>(grid_.size()); pos++)
    {
        if (grid_[pos] != 'O') continue;

        int first = pos;
        while (first % rowSize_ > 0 && !isWall(first - 1)) first--;
        int last = pos;
        while (last % rowSize_ < rowSize_ - 2 && !isWall(last + 1)) last++;

        lanes_.push_back({first, last - first + 1, pos == first});
        grid_[pos] = '.';
    }

    for (const Lane& lane : lanes_)
    {
        int lanePeriod = std::max(1, 2 * (lane.length - 1));
        period_ = std::lcm(period_, lanePeriod);
    }

    computeDistances();
    reset();
}

void Environment::reset()
{
    playerPos_ = startPos_;
    step_ = 0;
}

bool Environment::isWall(int pos) const
{
    if (pos < 0 || pos >= static_cast<int>(grid_.size())) return true;
    return grid_[pos] == '#' || grid_[pos] == '\n';
}

int Environment::move(int pos, int action) const
{
    int col = pos % rowSize_;
    int next = pos;
    switch (action)
    {
    case ACTION_LEFT:  next = col > 0 ? pos - 1 : -1; break;
    case ACTION_RIGHT: next = pos + 1; break;
    case ACTION_DOWN:  next = pos + rowSize_; break;
    case ACTION_UP:    next = pos - rowSize_; break;
    default: break;
    }
    return isWall(next) ? pos : next;
}

int Environment::enemyPos(const Lane& lane, int step) const
{
    if (lane.length <= 1) return lane.first;

    int lanePeriod = 2 * (lane.length - 1);
    int t = step % lanePeriod;
    int offset = t < lane.length ? t : lanePeriod - t;
    if (!lane.startsAtFirst) offset = lane.length - 1 - offset;
    return lane.first + offset;
}

bool Environment::hitsEnemy(int oldPos, int newPos, int oldStep, int newStep) const
{
    for (const Lane& lane : lanes_)
    {
        int enemyBefore = enemyPos(lane, oldStep);
        int enemyAfter = enemyPos(lane, newStep);
        if (enemyAfter == newPos) return true;
        // player and enemy swapped cells during the same step
        if (enemyBefore == newPos && enemyAfter == oldPos) return true;
    }
    return false;
}

void Environment::computeDistances()
{
    distToExit_.assign(grid_.size(), UNREACHABLE);
    std::queue<int> frontier;
    distToExit_[exitPos_] = 0;
    frontier.push(exitPos_);

    while (!frontier.empty())
    {
        int pos = frontier.front();
        frontier.pop();
        for (int action = ACTION_LEFT; action < ACTIONS_COUNT; action++)
        {
            int next = move(pos, action);
            if (next == pos || distToExit_[next] != UNREACHABLE) continue;
            distToExit_[next] = distToExit_[pos] + 1;
            frontier.push(next);
        }
    }
}

Environment::StepResult Environment::step(int action)
{
    int oldPos = playerPos_;
    int newPos = move(oldPos, action);
    int oldStep = step_;

    playerPos_ = newPos;
    step_++;

    if (newPos == exitPos_)
    {
        return {EXIT_REWARD, true, true, false};
    }
    if (hitsEnemy(oldPos, newPos, oldStep, step_))
    {
        return {DEATH_REWARD, true, false, true};
    }

    // potential-based shaping: rewards getting closer to the exit without
    // changing which policy is optimal
    double shaping = 0.0;
    if (distToExit_[oldPos] != UNREACHABLE && distToExit_[newPos] != UNREACHABLE)
    {
        shaping = SHAPING_SCALE * (distToExit_[oldPos] - distToExit_[newPos]);
    }
    return {STEP_REWARD + shaping, false, false, false};
}

int Environment::stateIndex() const
{
    return playerPos_ * period_ + (step_ % period_);
}

int Environment::stateCount() const
{
    return static_cast<int>(grid_.size()) * period_;
}

std::string Environment::render() const
{
    std::string frame = grid_;
    for (const Lane& lane : lanes_)
    {
        frame[enemyPos(lane, step_)] = 'O';
    }
    frame[playerPos_] = 's';
    return frame;
}

QLearningAgent::QLearningAgent(int stateCount, const Params& params)
    : params_(params)
    , table_(static_cast<size_t>(stateCount) * ACTIONS_COUNT, 0.0)
    , rng_(params.seed)
{
}

int QLearningAgent::bestAction(int state) const
{
    int best = 0;
    for (int action = 1; action < ACTIONS_COUNT; action++)
    {
        if (q(state, action) > q(state, best)) best = action;
    }
    return best;
}

int QLearningAgent::chooseAction(int state, double epsilon)
{
    std::uniform_real_distribution<double> coin(0.0, 1.0);
    if (coin(rng_) < epsilon)
    {
        std::uniform_int_distribution<int> anyAction(0, ACTIONS_COUNT - 1);
        return anyAction(rng_);
    }
    return bestAction(state);
}

void QLearningAgent::train(Environment& env, bool verbose)
{
    const int reportEvery = std::max(1, params_.episodes / 10);
    int successes = 0;

    for (int episode = 0; episode < params_.episodes; episode++)
    {
        double progress = static_cast<double>(episode) / std::max(1, params_.episodes - 1);
        double epsilon = params_.epsilonStart + (params_.epsilonEnd - params_.epsilonStart) * progress;

        env.reset();
        int state = env.stateIndex();

        for (int t = 0; t < params_.maxStepsPerEpisode; t++)
        {
            int action = chooseAction(state, epsilon);
            Environment::StepResult result = env.step(action);
            int nextState = env.stateIndex();

            double target = result.reward;
            if (!result.done)
            {
                target += params_.gamma * q(nextState, bestAction(nextState));
            }
            q(state, action) += params_.alpha * (target - q(state, action));

            state = nextState;
            if (result.done)
            {
                if (result.reachedExit) successes++;
                break;
            }
        }

        if (verbose && (episode + 1) % reportEvery == 0)
        {
            bool reached = false;
            std::vector<int> path = greedyRollout(env, reached);
            std::cout << "episode " << episode + 1 << "/" << params_.episodes
                      << "  epsilon " << epsilon
                      << "  exits in last block " << successes << "/" << reportEvery
                      << "  greedy policy: " << (reached ? "reaches exit in " : "fails after ")
                      << path.size() << " steps" << std::endl;
            successes = 0;
        }
    }
}

std::vector<int> QLearningAgent::greedyRollout(Environment& env, bool& reachedExit) const
{
    std::vector<int> actions;
    reachedExit = false;
    env.reset();

    for (int t = 0; t < params_.maxStepsPerEpisode; t++)
    {
        int action = bestAction(env.stateIndex());
        actions.push_back(action);
        Environment::StepResult result = env.step(action);
        if (result.done)
        {
            reachedExit = result.reachedExit;
            break;
        }
    }

    env.reset();
    return actions;
}
