#pragma once
#include "../Board/Move.h"
#include "../Board/type_definitions.h"

#include <vector>

struct MCTNode {
private:
    MCTNode *parent;
public:
    std::vector<MCTNode> children;
    u64 visits;
    double score;
    Move move;
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
};