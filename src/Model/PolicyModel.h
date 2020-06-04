#pragma once
#include <iostream>
#include <vector>

#include <torch/torch.h>

#include "../Board/BitBoard.h"
#include "../Board/type_definitions.h"
#include "../Board/Move.h"

const int input_size = 13 * 64 + 4 + 8; //Bitboard pieces, castling and en passant
const int output_size = 64 * 67; //From and to square
const int linear1_size = 700;
const int conv1_size = 700;

const int BATCH_SIZE = 1;
const int ITERATIONS = 10000;

const bool USE_GPU = true;

struct NeuralNet : torch::nn::Module {
    torch::Device device = torch::kCPU; 

    torch::nn::Linear input_weights, output_weights, linear1;
    //nn::Conv2d conv1, conv2;
    torch::Tensor input;
    torch::Tensor output;
    float* output_ptr;

    NeuralNet() :
            input_weights(register_module("input",torch::nn::Linear(input_size,linear1_size))),
            linear1(register_module("linear1",torch::nn::Linear(linear1_size,linear1_size))),
            output_weights(register_module("output",torch::nn::Linear(linear1_size,output_size)))
    {

    }
    torch::Tensor forward(torch::Tensor& input) {
        torch::NoGradGuard no_grad;
        auto x = torch::relu(input_weights(input));
        x = torch::relu(linear1(x));
        //x = torch::sigmoid(hidden_weights2(x));
        x = output_weights(x);
        return x;
    }
};

/// @brief This class wraps a PyTorch Neural Network used for guiding Monte Carlo Policy
class PolicyModel
{
private:
    /* data */
    NeuralNet model = NeuralNet();

    float getMoveValue(Move& move);

    void forward();

    void setInputToBoard(BitBoard& board);

public:

    void train(torch::Tensor& input, torch::Tensor& target);
    void test();

    void forwardPolicyMoveSort(BitBoard& board, Move* moves_begin, Move* moves_end);

    PolicyModel(/* args */);
    ~PolicyModel();
};