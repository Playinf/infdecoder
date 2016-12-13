/* innltm.cpp */
#include <string>
#include <vector>
#include <stdexcept>
#include <nnltm.h>
#include <innltm.h>
#include <vocab.h>
#include <symbol.h>
#include <lexical_model.h>
#include <language_model.h>

inline float floor_score(float score)
{
    const float min_score = -100.0f;

    return std::max(score, min_score);
}

innltm::innltm()
{
    order = 0;
    window_size = 0;
    model = nullptr;
    input_vocab = nullptr;
    output_vocab = nullptr;
}

innltm::~innltm()
{
    if (model != nullptr)
        delete model;
}

unsigned int innltm::get_window_size() const
{
    return window_size;
}

void innltm::load(const char* name)
{
    if (model != nullptr) {
        delete model;
    }

    model = new infinity::lm::nnltm;
    model->load(name);
    input_vocab = model->get_input_vocab();
    output_vocab = model->get_output_vocab();

    if (window_size + 1 != order) {
        std::string msg = "incompatible nnltm order";
        throw std::runtime_error(msg);
    }

    if (model->get_window_size() != window_size) {
        std::string msg = "incompatible nnltm window size";
        throw std::runtime_error(msg);
    }

    model->precompute();
}

void innltm::set_window_size(unsigned int n)
{
    window_size = n;
    order = window_size + 1;
}
//#include <iostream>
// word-window predicted-word
float innltm::probability(const std::string** input)
{
    double score;
    unsigned int iunk_id = input_vocab->get_id("<unk>");
    unsigned int ounk_id = output_vocab->get_id("<unk>");
    const std::string** window = input;
    const std::string* label = input[window_size];

    // special treatment
    if (*label == "<s>" || *label == "</s>")
        return 0.0f;

    unsigned int* ngram = new unsigned int[order];

    for (unsigned int i = 0; i < window_size; i++) {
        auto iter = input_vocab->find(*window[i]);

        if (iter != input_vocab->end())
            ngram[i] = iter->second;
        else
            ngram[i] = iunk_id;
    }

    auto iter = output_vocab->find(*label);

    if (iter != output_vocab->end())
        ngram[order - 1] = iter->second;
    else
        ngram[order - 1] = ounk_id;

    score = model->ngram_prob(ngram);
/*
    for (unsigned int i = 0; i < window_size + 1; i++)
        std::cout << *input[i] << " ";
    std::cout << score << std::endl;*/

    delete[] ngram;

    return floor_score(score);
}
