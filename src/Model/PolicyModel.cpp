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
    else
        std::cout << "Using CPU for PyTorch model." << std::endl; 
    

    //Setting which increases speed for static network sizes (which our is)
    at::globalContext().setBenchmarkCuDNN(true);

    //Transfer model over to the relevant device
    model->to(device);
    //optim_ptr = std::shared_ptr<torch::optim::SGD>(new torch::optim::SGD(model->parameters(), torch::optim::SGDOptions(1e-1)));
    optim_ptr = std::shared_ptr<torch::optim::Adam>(new torch::optim::Adam(model->parameters()));
}

/// @brief Sets training mode on the model. It will now be optimized for training/backprop
void PolicyModel::setTrainingMode()
{
    //Set the model to training mode
    model->train();
    //Setup training optimizer
    if (!optimizer_initialized)
    {
        //optim_ptr = std::shared_ptr<torch::optim::SGD>(new torch::optim::SGD(model->parameters(), torch::optim::SGDOptions(1e-1)));
        optim_ptr = std::shared_ptr<torch::optim::Adam>(new torch::optim::Adam(model->parameters()));
        optimizer_initialized = true;
    }
}

/// @brief Sets evaluation mode on the model. It will now be optimized for evaluation/forward
void PolicyModel::setEvaluationMode()
{
    //Set model to evaluation mode
    model->eval();
    //Do not perform gradient descent here, no grad guard.
    torch::NoGradGuard no_grad;
}

/// @brief Train the model based on the input and output, previously set by the trainBatches
void PolicyModel::train()
{
    training_output = model->forward(training_input);
    training_loss = torch::mse_loss(training_output, training_target);

    training_loss.backward();
    optim_ptr->step();
    optim_ptr->zero_grad();

    if (save_checkpoints && (training_iteration_counter % ITERATIONS_PER_CHECKPOINT == 0)) //Time to save a training checkpoint/snapshot
    {
        saveCheckpoint(checkpoint_counter);
        checkpoint_counter++;
    }
    average_loss += training_loss.item<float>();
    if (training_iteration_counter % 500 == 0)
    {
        std::cout << "Avg Loss: " << (average_loss / 500) << " (" << (float(training_iteration_counter)/ITERATIONS)*100 << "%, i=" << training_iteration_counter << ")\n";
        average_loss = 0;
    }

    training_iteration_counter++;
}

/// @brief Set the model input and target output based on the current batch, specified by the input_batches array. Then train
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
    train();
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
    eval_output = eval_output.to(torch::kCPU);
    eval_output_ptr = eval_output.data_ptr<float>();

    //For batches, model.output.to(torch:kCPU)[x].data_ptr...
}

/// @brief Convert a move object to its output neuron index counterpart
int moveToOutputIndex(Move move, bool color) //Rotate based on color
{
    int index;
    if (!color)
        move.rotateForColor();
    
    if (move.upgrade() != 0)
        index = 64*64 - 1  + ((move.to() % 8) * 4 + (move.upgrade() - 1));
    else
    {  
        index = move.from() * 64 + move.to();
    }
    
    return index;
}

/// @brief Convert a output neuron index into its counterpart move object
Move outputIndexToMove(int index, bool color) //Rotate based on color
{
    //Fix colors
    Move move;

    if (index >= 64*64) //Promotion
    {
        move.setUpgrade((index % 4) + 2);
        int sq = (index - 64 * 64) / 8;
        move.setTo(56 + sq); //White
        move.setFrom(48 + sq);
    }
    else
    {
        move.setFrom(index / 64);
        move.setTo(index % 64);
    }

    if (!color)
        move.rotateForColor();
    
    return move;
}

/// @brief Return the output value from the network for this move. Rotates the moves based on color
float getMoveValue(Move& move, float* output_ptr, bool color) //Rotate based on color
{
    return output_ptr[moveToOutputIndex(move, color)];
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

///@brief Forward propagate an input and set the output 
void PolicyModel::evaluate(BitBoard& board)
{
    //Do not perform gradient descent here, no grad guard.
    torch::NoGradGuard no_grad;
    //Set the input tensor to the bitboard
    setInputToBoard(board);
    eval_input = torch::from_blob(input_array, {input_size}).to(device);

    //Forward propagate the network with this input
    forward();
}

float PolicyModel::evalWinrate(BitBoard& board)
{
    evaluate(board);

    return eval_output_ptr[output_size-1];
}

/// @brief This function sorts moves based on the output of the policy network
void PolicyModel::forwardPolicyMoveSort(BitBoard& board, Move* moves_begin, Move* moves_end)
{
    //Forward propagate the network with this input
    evaluate(board);
    //Sort the moves based on the policy output generated by the network
    std::sort(moves_begin, moves_end, policyModelSortCompFunctor(eval_output_ptr, board.toMove()));
}

/// @brief This function returns all moves above a certain cutoff in hte output of the policy network
std::vector<Move> PolicyModel::getOutputMoves(BitBoard& board, float cutoff)
{
    //Forward propagate the network with this input
    evaluate(board);

    //Find all output neuron moves with a value above the cutoff and add them to the move list.
    std::vector<Move> moves = std::vector<Move>();
    while (true)
    {
        int index = std::max_element(eval_output_ptr, eval_output_ptr+output_size)-eval_output_ptr;
        float value = eval_output_ptr[index];
        if (value < cutoff)
            break;
        else
            eval_output_ptr[index] = 0;
        
        moves.push_back(outputIndexToMove(index, board.toMove()));
    }
    return moves;
}

/// @brief Save the model to file
void PolicyModel::save(std::string filename)
{
    model->to(torch::kCPU);
    std::cout << "Saving model to file " << filename << std::endl;
    torch::save(model, MODEL_FOLDER + filename + ".pt");
    torch::save(*optim_ptr, MODEL_FOLDER + filename + "_optimizer.pt");
    model->to(device);
}

/// @brief Load the model from file
void PolicyModel::load(std::string filename)
{
    model->to(torch::kCPU);
    std::cout << "Loading model from file " << filename << std::endl;
    torch::load(model, MODEL_FOLDER + filename + ".pt");
    torch::load(*optim_ptr, MODEL_FOLDER + filename + "_optimizer.pt");
    optimizer_initialized = true;
    model->to(device);
}

/// @brief Save a checkpoint to file
void PolicyModel::saveCheckpoint(int checkpoint)
{
    std::string path = "c" + std::to_string(checkpoint) + "_" + MODEL_NAME;
    save(path);
}

/// @brief Load a checkpoint from file
void PolicyModel::loadCheckpoint(int checkpoint)
{
    std::string path = "c" + std::to_string(checkpoint) + "_" + MODEL_NAME;
    load(path);
    checkpoint_counter = checkpoint + 1;
    training_iteration_counter = ITERATIONS_PER_CHECKPOINT*(checkpoint_counter) + 1;
}

/// @brief Load the most recent checkpoint from file
void PolicyModel::loadMostRecentCheckpoint()
{
    int max_checkpoint_found = -1;

    for(auto p : std::filesystem::recursive_directory_iterator(MODEL_FOLDER))
    {
        if(p.path().extension() == ".pt")
        {
            if (p.path().string().find(MODEL_NAME) != std::string::npos && p.path().string().find("optimizer") == std::string::npos)
            {
                int checkpoint = StringHelpers::extractFirstNumber(p.path().string());
                if (checkpoint > max_checkpoint_found)
                    max_checkpoint_found = checkpoint;
            }
        }
    }
    if (max_checkpoint_found != -1)
        loadCheckpoint(max_checkpoint_found);
    else
        std::cout << "Error: Could not locate any saved checkpoints for this model." << std::endl;
    
}

/// @brief Remove training checkpoints for this model
void PolicyModel::resetTrainingCheckpoints()
{
    for(auto p : std::filesystem::recursive_directory_iterator(MODEL_FOLDER))
    {
        if(p.path().extension() == ".pt")
        {
            if (p.path().string().find(MODEL_NAME) != std::string::npos)
            {
                std::filesystem::remove(p.path());
            }
        }
    }
}

PolicyModel::~PolicyModel()
{
}
