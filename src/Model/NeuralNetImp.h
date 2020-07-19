#pragma once
#include "torch/torch.h"

const int input_size = 13 * 64 + 4 + 8; //Bitboard pieces, castling and en passant
const int output_size = 64 * 64 + 32 + 1; //From and to square plus promotions and evaluation neuron
const int linear_size = 13 * 64 + 4 + 8;

struct NeuralNetImpl : torch::nn::Module {

    torch::nn::Linear input_weights, output_weights, linear1;//, linear2, linear3;
    //nn::Conv2d conv1, conv2;

    NeuralNetImpl();

    torch::Tensor forward(torch::Tensor& input);
};

TORCH_MODULE_IMPL(NeuralNet, NeuralNetImpl);
