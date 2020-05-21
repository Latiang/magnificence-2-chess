#include "PolicyModel.h"

PolicyModel::PolicyModel(/* args */)
{
    torch::manual_seed(1);

    if (USE_GPU && torch::cuda::is_available()) {
      std::cout << "Using CUDA for model." << std::endl;
      device = torch::kCUDA;
    }
}

void PolicyModel::train(torch::Tensor &input, torch::Tensor &target)
{

    std::cout << "Training..." << std::endl;

    auto optimizer = torch::optim::SGD(model.parameters(), torch::optim::SGDOptions(1e-4));

    torch::Tensor output, loss;

    model.train();
    model.to(device);

    for (size_t i = 0; i < ITERATIONS; i++)
    {
        output = model.forward(input);
        loss = torch::mse_loss(output, target);
        //std::cout << target << std::endl;
        loss.backward();
        optimizer.step();
        optimizer.zero_grad();
        if(i%10 == 0)
            std::cout << "Loss:" << loss << "\n";
    }
    std::cout << "Training complete" << std::endl;
}

void PolicyModel::test()
{
    //Train
    auto input = torch::ones({BATCH_SIZE, input_size}, device);
    auto target = torch::randn({BATCH_SIZE, output_size}, device);
    train(input, target);

    //Test

    auto out = model.forward(input);

    std::cout << "Expected: " << target << std::endl;
    std::cout << "Result: " << out << std::endl;
}

/// @brief Set the network model input to a bitboard. Only supports 'one' color, rotates the board to make black be white.
void PolicyModel::setInputToBoard(BitBoard& board)
{
    float input_array[input_size];
    int counter = 0;

    bool color = board.toMove();
    if (color) //White turn
    {
        for (size_t i = 0; i < 13; i++)
        {
            u64 bb = board.bitboard().pieces[i];
            for (int j = 0; j < 64; j++) {
                input_array[counter] = (bb >> j) & 1;
                counter++;
            }
        }
    }
    else //Black turn. Rotate board, black pieces are white and vice versa
    {
        u64 bb = byteswap(board.bitboard().pieces[0]); //Empty squares. Byte swap rotates board
        for (int j = 0; j < 64; j++) {
            input_array[counter] = (bb >> j) & 1;
            counter++;
        }
        for (size_t i = 7; i < 13; i++) //Black pieces are now first, ie white
        {
            u64 bb = byteswap(board.bitboard().pieces[i]);
            for (int j = 0; j < 64; j++) {
                input_array[counter] = (bb >> j) & 1;
                counter++;
            }
        }
        for (size_t i = 1; i < 7; i++) //White pieces are now second, ie black
        {
            u64 bb = byteswap(board.bitboard().pieces[i]);
            for (int j = 0; j < 64; j++) {
                input_array[counter] = (bb >> j) & 1;
                counter++;
            }
        }
        
    }
    //Castling. Swap depending on color
    u8 castling = board.getCastling();
    if (color)
    {
        input_array[counter++] = (castling >> 3) & 1;
        input_array[counter++] = (castling >> 2) & 1;
        input_array[counter++] = (castling >> 1) & 1;
        input_array[counter++] = (castling >> 0) & 1;
    }
    else
    {
        input_array[counter++] = (castling >> 1) & 1;
        input_array[counter++] = (castling >> 0) & 1;
        input_array[counter++] = (castling >> 3) & 1;
        input_array[counter++] = (castling >> 2) & 1;
    }
    

    for (size_t i = 0; i < 8; i++) //Zero en passant section
    {   
        input_array[counter] = 0;
        counter++;
    }

    if (board.getEP() != 8) //En passant square
        input_array[counter - 8 + board.getEP()] = 1;
    

    //std::memcpy(input_array_ptr, board.bitboard().pieces[0])
    test_input = torch::from_blob(input_array, {input_size}, at::kFloat);
    //tensor_image = tensor_image.to(at::kFloat);
}

void PolicyModel::testForward()
{
    test_output = model.forward(test_input);
    //std::cout << test_output.index({1}) << std::endl;
}

PolicyModel::~PolicyModel()
{
}
