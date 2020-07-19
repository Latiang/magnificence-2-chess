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
#include "MCTNode.h"
#include "../Board/type_definitions.h"
#include "../Interface/BoardConversions.h"
#include "Engine.h"

#include "../Model/PolicyModel.h"
#include "../Model/TrainingHelper.h"

#include <chrono>
#include <iostream>
#include <cmath>

/**
 * @brief Monte Carlo Engine class. Inherits from base class Engine.
 * 
 **/
class EngineMCT : public Engine
{
private:
    Move move_space[30000];

    std::vector<Move> move_storage;

    Winner simulateGame();

    void expandTree(MCTNode &node);

    void searchTree(MCTNode &node);

    double scorePosition();

    void trainingSearchRecursive(MCTNode& node, TrainingHelper& t_helper);

public:
    PolicyModel& model;

    void search() override;

    void trainingSearch();


    EngineMCT(PolicyModel& model);

    ~EngineMCT();
};