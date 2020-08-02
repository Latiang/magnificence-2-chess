#pragma once
#include "torch/torch.h"

const int input_size = 13 * 64 + 4 + 8; //Bitboard pieces, castling and en passant
const int output_size = 64 * 64 + 32 + 1; //From and to square plus promotions and evaluation neuron
const int linear_size = 13 * 64 + 4 + 8;
const int conv_size = 256;

struct NeuralNetImpl : torch::nn::Module {

    torch::nn::Linear linear1, linear2, linear3, input_weights, output_weights;
    //torch::nn::ConvTranspose2d conv1;

    NeuralNetImpl();

    torch::Tensor forward(torch::Tensor& input);
};

//input_weights, output_weights,

TORCH_MODULE_IMPL(NeuralNet, NeuralNetImpl);
