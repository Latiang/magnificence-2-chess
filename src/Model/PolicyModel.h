#pragma once
#include <iostream>
#include <vector>
#include <string>
#include <filesystem>

#include "NeuralNetImp.h"

#include "../Board/BitBoard.h"
#include "../Board/type_definitions.h"
#include "../Board/Move.h"

#include "../Interface/StringHelpers.h"

const int BATCH_SIZE = 1;
const int ITERATIONS = 1000000;
const int ITERATIONS_PER_CHECKPOINT = 25000;

const bool USE_GPU = true;

const std::string MODEL_FOLDER = "models/";
const std::string MODEL_NAME = "modeleval";

int moveToOutputIndex(Move move, bool color);
Move outputIndexToMove(int index, bool color);

struct TrainingNode
{
    std::vector<Move> moves;
    std::vector<float> win_rates;
    BitBoard board;
    float model_output[output_size] = {};
    bool color = true;
    TrainingNode() {
        //Move best_move;
        //e2e4
        //best_move.setFrom(12);
        //best_move.setTo(28);
        //d2d4
        //Move second_best_move;
        //second_best_move.setFrom(11);
        //second_best_move.setTo(27);
        //a7a8 pawn to queen promotion
        //best_move.setFrom(48);
        //best_move.setTo(56);
        //best_move.setUpgrade(5);
        //moves.push_back(best_move);
        //moves.push_back(second_best_move);
        //win_rates.push_back(1);
        //win_rates.push_back(0.7);
        //convertToNeuralOutput();
    }
    TrainingNode(BitBoard board, float winrate)
    {
        model_output[output_size-1] = winrate;
        this->board = board;
    }

    TrainingNode(BitBoard board, Move* moves_begin, Move* moves_end, float winrate) {
        int move_count = moves_end - moves_begin;
        while (moves_begin < moves_end)
        {
            win_rates.push_back(1);
            moves.push_back(*moves_begin);
            moves_begin++;
        }

        convertToNeuralOutput();

        model_output[output_size-1] = winrate;
        this->board = board;
    }

    void convertToNeuralOutput()
    {
        for (size_t i = 0; i < moves.size(); i++)
        {
            model_output[moveToOutputIndex(moves[i], color)] = win_rates[i];
        }
    }
};

/// @brief This class wraps a PyTorch Neural Network used for guiding Monte Carlo Policy
class PolicyModel
{
private:
    /* data */
    NeuralNet model = NeuralNet();
    torch::Device device = torch::kCPU; 

    torch::Tensor training_input, training_output, training_target, training_loss;

    bool optimizer_initialized = false;

    torch::Tensor eval_input;
    torch::Tensor eval_output;

    float input_array[input_size];

    //Forward propagation helpers
    float getMoveValue(Move& move);

    void forward();

    std::shared_ptr<torch::optim::Adam> optim_ptr;

    void setInputToBoard(BitBoard& board);

    void train();

public:

    bool save_checkpoints = true;
    int training_iteration_counter = 1;
    float average_loss = 0;
    int checkpoint_counter = 0;

    TrainingNode train_data[BATCH_SIZE];
    float* eval_output_ptr;

    void trainBatches();

    void forwardPolicyMoveSort(BitBoard& board, Move* moves_begin, Move* moves_end);

    std::vector<Move> getOutputMoves(BitBoard& board, float cutoff = 0.7);

    void setTrainingMode();

    void setEvaluationMode();

    void evaluate(BitBoard& board);

    float evalWinrate(BitBoard& board);

    PolicyModel(/* args */);
    ~PolicyModel();

    //Functions for loading and saving models
    void save(std::string filename = MODEL_NAME);

    void load(std::string filename = MODEL_NAME);

    void saveCheckpoint(int checkpoint);

    void loadCheckpoint(int checkpoint);

    void loadMostRecentCheckpoint();

    void resetTrainingCheckpoints();
};