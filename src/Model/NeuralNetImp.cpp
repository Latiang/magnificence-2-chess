#include "NeuralNetImp.h"

NeuralNetImpl::NeuralNetImpl() :
        //Comment line below to disable convolutions
        conv1(torch::nn::Conv2dOptions(1, 32, 3).padding(1)),
        linear1(torch::nn::Linear(5280,linear_size)),
        linear2(torch::nn::Linear(linear_size,output_size))
        //Uncomment these lines to make 3 layers plus input and output layer
        //linear3(torch::nn::Linear(linear_size,output_size))
        //input_weights(torch::nn::Linear(input_size,linear_size)),
        //output_weights(torch::nn::Linear(linear_size,outpu_size))
{
    //Comment line below to disable convolutions
    register_module("conv1", conv1);
    register_module("linear1", linear1);
    register_module("linear2", linear2);
    //Uncomment these lines to make 3 layers plus input and output layer
    //register_module("linear3", linear3);6
    //register_module("input", input_weights);
    //register_module("output", output_weights);
}

// conv2(register_module("conv2",torch::nn::Conv2d(torch::nn::Conv2dOptions(1, 10, 3).padding(1)))),

torch::Tensor NeuralNetImpl::forward(torch::Tensor& input) {
    //Comment to disable convolutions
    auto x = torch::relu(conv1(input));
    x = torch::max_pool2d(x, 3, 2);
    x = x.view({1, 5280});

    //Uncomment for three layers and input/output layer
    //auto x = torch::relu(input_weights(input));
    x = torch::relu(linear1(x));
    x = torch::relu(linear2(x));
    //x = torch::relu(linear3(x));

    //x = output_weights(x);
    return x;
}