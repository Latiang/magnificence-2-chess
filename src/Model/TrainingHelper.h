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

    void sendBatch(BitBoard board, Move move, float eval)
    {
        model.train_data[batch] = TrainingNode(board, move, eval);
        batch++;
        if (batch == BATCH_SIZE)
        {
            model.trainBatches();
            batch = 0;
        }
    }

    void sendBatch(BitBoard board, MCTNode parent, float eval)
    {
        model.train_data[batch] = TrainingNode(board, parent.children, eval);
        batch++;
        if (batch == BATCH_SIZE)
        {
            model.trainBatches();
            batch = 0;
        }
    }
};