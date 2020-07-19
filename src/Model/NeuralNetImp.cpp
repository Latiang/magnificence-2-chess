#include "NeuralNetImp.h"

NeuralNetImpl::NeuralNetImpl() :
        input_weights(register_module("input",torch::nn::Linear(input_size,linear_size))),
        linear1(register_module("linear1",torch::nn::Linear(linear_size,linear_size))),
        //linear2(register_module("linear2",torch::nn::Linear(linear_size,linear_size))),
        //linear3(register_module("linear3",torch::nn::Linear(linear_size,linear_size))),
        output_weights(register_module("output",torch::nn::Linear(linear_size,output_size)))
{

}

torch::Tensor NeuralNetImpl::forward(torch::Tensor& input) {
    auto x = torch::relu(input_weights(input));
    x = torch::relu(linear1(x));
    //x = torch::relu(linear2(x));
    //x = torch::relu(linear3(x));

    x = output_weights(x);
    return x;
}