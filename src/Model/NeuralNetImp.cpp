#include "NeuralNetImp.h"

NeuralNetImpl::NeuralNetImpl() :
        input_weights(torch::nn::Linear(input_size,linear_size)),
        //conv1(torch::nn::ConvTranspose2dOptions(input_size, conv_size, 3).bias(false)),
        linear1(torch::nn::Linear(linear_size,linear_size)),
        linear2(torch::nn::Linear(linear_size,linear_size)),
        linear3(torch::nn::Linear(linear_size,linear_size)),
        output_weights(torch::nn::Linear(linear_size,output_size))

{
    //register_module("conv1", conv1);
    register_module("input", input_weights);
    register_module("linear1", linear1);
    register_module("linear2", linear2);
    register_module("linear3", linear3);
    register_module("output", output_weights);
}

// conv2(register_module("conv2",torch::nn::Conv2d(torch::nn::Conv2dOptions(1, 10, 3).padding(1)))),

torch::Tensor NeuralNetImpl::forward(torch::Tensor& input) {
    //auto x = torch::relu(conv1(input));

    auto x = torch::relu(input_weights(input));
    x = torch::relu(linear1(x));
    x = torch::relu(linear2(x));
    x = torch::relu(linear3(x));

    x = output_weights(x);
    return x;
}