#include "PolicyModel.h"


PolicyModel::PolicyModel(/* args */)
{
    //Set seed to 1 to always get the same random for debugging purposes.
    torch::manual_seed(1);
    //Enable evaluation mode at start
    setEvaluationMode();

    //Use CUDA (GPU) if available. Else use CPU.
    if (USE_GPU && torch::cuda::is_available()) {
      std::cout << "Using CUDA (GPU) for PyTorch model." << std::endl;
      device = torch::kCUDA;
    }

    //Setting which increases speed for static network sizes (which our is)
    at::globalContext().setBenchmarkCuDNN(true);

    //Transfer model over to the relevant device
    model->to(device);
}

/// @brief Sets training mode on the model. It will now be optimized for training/backprop
void PolicyModel::setTrainingMode()
{
    //Set the model to training mode
    model->train();
    //Setup training optimizer
    optim_ptr = std::shared_ptr<torch::optim::SGD>(new torch::optim::SGD(model->parameters(), torch::optim::SGDOptions(1e-2)));
}

/// @brief Sets evaluation mode on the model. It will now be optimized for evaluation/forward
void PolicyModel::setEvaluationMode()
{
    //Set model to evaluation mode
    model->eval();
    //Do not perform gradient descent here, no grad guard.
    torch::NoGradGuard no_grad;
}

void PolicyModel::train()
{
    training_output = model->forward(training_input);
    training_loss = torch::mse_loss(training_output, training_target);

    training_loss.backward();
    optim_ptr->step();
    optim_ptr->zero_grad();

}

void PolicyModel::trainBatches()
{
    //Training input
    std::vector<float> input_batches;
    for (size_t i = 0; i < BATCH_SIZE; i++)
    {
        setInputToBoard(train_data[i].board);
        input_batches.insert(input_batches.end(), input_array, input_array + input_size);
    }
    training_input = torch::from_blob(&input_batches[0], {BATCH_SIZE, input_size}).to(device);
    //training_input = torch::ones({BATCH_SIZE, input_size}, model.device);
    //Target output
    std::vector<float> output_batches;
    for (size_t i = 0; i < BATCH_SIZE; i++)
    {
        output_batches.insert(output_batches.end(), train_data[i].model_output, train_data[i].model_output + output_size);
    }
    training_target = torch::from_blob(&output_batches[0], {BATCH_SIZE, output_size}).to(device);

    for (size_t i = 0; i < 10000; i++)
    {
        train();
        if (i%100 == 0) 
        {
            std::cout << training_loss << std::endl;
        }
    }
}


void PolicyModel::trainTest()
{
    //Train
    trainBatches();
    std::cout << "Final Loss: " << training_loss << std::endl;
}

/// @brief Set the network model input to a bitboard. Only supports 'one' color, rotates the board to make black be white.
void PolicyModel::setInputToBoard(BitBoard& board)
{
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
    //tensor_image = tensor_image.to(at::kFloat);
}

/// @brief Forward propagate the neural network model. Stores a floating point pointer to the output in model.output_ptr
void PolicyModel::forward()
{
    //Evaluation mode, do not perform gradient descent here, no grad guard.
    torch::NoGradGuard no_grad;
    eval_output = model->forward(eval_input);
    eval_output_ptr = eval_output.to(torch::kCPU).data_ptr<float>();
    //For batches, model.output.to(torch:kCPU)[x].data_ptr...
}

int getMoveOutputIndex(Move& move, bool color) //Rotate based on color
{
    int to_sq = color ? move.to() : (move.to() % 8 + (7 - move.to() / 8) * 8); 
    int from_sq = color ? move.from() : (move.from() % 8 + (7 - move.from() / 8) * 8);  
    int index = from_sq*64 + (to_sq + std::max(0, move.upgrade() - 1));
    //64 * 64 is not max. If there is a promotion, I want the to square to be in th
    
    return index;
}

/// @brief Return the output value from the network for this move. Rotates the moves based on color
float getMoveValue(Move& move, float* output_ptr, bool color) //Rotate based on color
{
    return output_ptr[getMoveOutputIndex(move, color)];
}

/// @brief This functor is used as a comparison operator for the std sort function for moves
struct policyModelSortCompFunctor
{
    bool color;
    float* output_ptr;
    policyModelSortCompFunctor(float* output_ptr, int color){
        this->output_ptr = output_ptr;
        this->color = color;
    }
    bool operator()(  Move& lhs, Move& rhs )  {
        return getMoveValue(lhs, output_ptr, color) > getMoveValue(rhs, output_ptr, color);
    }
        
};

/// @brief This function sorts moves based on the output of the policy network
void PolicyModel::forwardPolicyMoveSort(BitBoard& board, Move* moves_begin, Move* moves_end)
{
    //Do not perform gradient descent here, no grad guard.
    torch::NoGradGuard no_grad;
    //Set the input tensor to the bitboard
    setInputToBoard(board);
    eval_input = torch::from_blob(input_array, {input_size}).to(device);

    //Forward propagate the network with this input
    forward();
    
    //Sort the moves based on the policy output generated by the network
    std::sort(moves_begin, moves_end, policyModelSortCompFunctor(eval_output_ptr, board.toMove()));
}

void PolicyModel::save()
{
    torch::save(model, "model.pt");
}

void PolicyModel::load()
{
    torch::load(model, "model.pt");
}

PolicyModel::~PolicyModel()
{
}
