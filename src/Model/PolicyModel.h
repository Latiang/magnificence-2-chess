#pragma once
#include <iostream>
#include <vector>

#include <torch/torch.h>

#include "../Board/BitBoard.h"
#include "../Board/type_definitions.h"

const int input_size = 13 * 64 + 4 + 8; //Bitboard pieces, castling and en passant
const int output_size = 64 * 2; //From and to square
const int hidden_size = 700;

const int BATCH_SIZE = 1;
const int ITERATIONS = 10000;

const bool USE_GPU = true;

struct NeuralNet : torch::nn::Module {
    torch::nn::Linear input_weights, hidden_weights, output_weights;

    NeuralNet() :
            input_weights(register_module("input",torch::nn::Linear(input_size,hidden_size))),
            hidden_weights(register_module("hidden",torch::nn::Linear(hidden_size,hidden_size))),
            output_weights(register_module("output",torch::nn::Linear(hidden_size,output_size)))
    {

    }
    torch::Tensor forward(torch::Tensor& input) {
        auto x = torch::sigmoid(input_weights(input));
        x = torch::sigmoid(hidden_weights(x));
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
    torch::Device device = torch::kCPU; 
    NeuralNet model = NeuralNet();

public:
    torch::Tensor test_input;
    torch::Tensor test_output;

    void train(torch::Tensor& input, torch::Tensor& target);
    void test();

    void testForward();

    void setInputToBoard(BitBoard& board);

    PolicyModel(/* args */);
    ~PolicyModel();
};