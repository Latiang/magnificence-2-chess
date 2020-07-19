#pragma once
#include "PolicyModel.h"
#include "../Board/BitBoard.h"

class TrainingHelper
{
private:
    PolicyModel& model;
    int batch = 0;
public:
    TrainingHelper(PolicyModel& _model) : model(_model)
    {

    }

    void sendBatch(BitBoard board, MCTNode& node, float winrate)
    {
        model.train_data[batch] = TrainingNode(board, node.children, winrate);
        batch++;
        if (batch == BATCH_SIZE)
        {
            model.trainBatches();
            batch = 0;
        }
    }
};