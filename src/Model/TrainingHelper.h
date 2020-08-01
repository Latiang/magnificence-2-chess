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

    void sendBatch(BitBoard board, Move move, float winrate)
    {
        model.train_data[batch] = TrainingNode(board, move, winrate);
        batch++;
        if (batch == BATCH_SIZE)
        {
            model.trainBatches();
            batch = 0;
        }
    }
};