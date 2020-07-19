/**
 * @file EngineAB.h
 * @author William Sandström and Harald Bjurulf
 * @brief Contains the class EngineMCT
 * @version 0.1
 * @date 2020-05-05
 * 
 * @copyright Copyright (c) 2020
 * 
 */
#pragma once
#include "../Board/type_definitions.h"
#include "../Interface/BoardConversions.h"
#include <iostream>
#include <cmath>
#include "Engine.h"
#include <chrono>




struct MCTNode {
private:
    MCTNode *parent;
public:
    u64 visits;
    double score;
    Move move;
    double value;
    MCTNode(Move move_in) {
        move = move_in;
        visits = 0;
        score = 0;
        parent = nullptr;
    }
    MCTNode(Move move_in, MCTNode *parent_in) {
        parent = parent_in;
        visits = 0;
        score = 0;
        move = move_in;
    }

    void addScore(double score_in) {
        visits += 1;
        score += score_in;
        if (parent) {
            parent->addScore(1 - score_in);
        }
    }

    void addChildren(Move *start, Move *end) {
        while (start != end) {
            children.push_back(MCTNode(*start, this));
            start++;
        }
    }

    friend bool operator<(const MCTNode& l ,const MCTNode &r) {
        return l.value < r.value;
    }

    friend bool operator>(const MCTNode& l ,const MCTNode &r) {
        return l.value > r.value;
    }

    void sortChildren() {
        std::sort(children.begin(), children.end(), std::greater<MCTNode>());
    }


    std::vector<MCTNode> children;
};



/**
 * @brief Monte Carlo Engine class. Inherits from base class Engine.
 * 
 **/
class EngineMCT : public Engine
{
private:
    Move move_space[30000];

    Move (*playout_policy)(Move*, Move*, BitBoard&);

    std::vector<Move> move_storage;

    Winner simulateGame();

    void expandTree(MCTNode &node);

    void searchTree(MCTNode &node);

    double scorePosition();
public:

    void search() override;


    EngineMCT(/* args */);
    EngineMCT(Move (*playout_policy_in)(Move*, Move*, BitBoard&));
    ~EngineMCT();
};