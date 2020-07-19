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

    void sendBatch(BitBoard board, Move* moves_begin, Move* moves_end, float winrate)
    {
        model.train_data[batch] = TrainingNode(board, moves_begin, moves_end, winrate);
        batch++;
        if (batch == BATCH_SIZE)
            model.trainBatches();
    }
};