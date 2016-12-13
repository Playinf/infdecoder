/* innjm.cpp */
#include <string>
#include <vector>
#include <stdexcept>
#include <nnjm.h>
#include <innjm.h>
#include <vocab.h>
#include <symbol.h>
#include <joint_model.h>
#include <language_model.h>

inline float floor_score(float score)
{
    const float min_score = -100.0f;

    return std::max(score, min_score);
}

innjm::innjm()
{
    context[0] = 0;
    context[1] = 0;
    model = nullptr;
    source_vocab = nullptr;
    target_vocab = nullptr;
    output_vocab = nullptr;
}

innjm::~innjm()
{
    if (model != nullptr)
        delete model;
}

unsigned int innjm::get_source_context() const
{
    return context[0];
}

unsigned int innjm::get_target_context() const
{
    return context[1];
}

void innjm::load(const char* name)
{
    if (model != nullptr) {
        delete model;
    }
    model = new infinity::lm::nnjm;
    model->load(name);
    source_vocab = model->get_source_vocab();
    target_vocab = model->get_target_vocab();
    output_vocab = model->get_output_vocab();

    if (model->get_source_context() != context[0]) {
        std::string msg = "incompatible nnjm source context";
        throw std::runtime_error(msg);
    }

    if (model->get_target_context() != context[1]) {
        std::string msg = "incompatible nnjm target context";
        throw std::runtime_error(msg);
    }

    model->precompute();
}

void innjm::set_source_context(unsigned int n)
{
    context[0] = n;
}

void innjm::set_target_context(unsigned int n)
{
    context[1] = n;
}
//#include <iostream>
// source-context target-context predicted-word
float innjm::probability(const std::string** input)
{
    double score;
    unsigned int sunk_id = source_vocab->get_id("<unk>");
    unsigned int tunk_id = target_vocab->get_id("<unk>");
    const std::string** source = input;
    const std::string** target = input + context[0];
    const std::string* label = input[context[0] + context[1]];

    // special treatment
    if (*label == "<s>" && get_type() == language_model_type::forward)
        return 0.0f;

    if (*label == "</s>" && get_type() == language_model_type::backward)
        return 0.0f;

    unsigned int order = context[0] + context[1] + 1;
    unsigned int* ngram = new unsigned int[order];

    for (unsigned int i = 0; i < context[0]; i++) {
        auto iter = source_vocab->find(*source[i]);

        if (iter != source_vocab->end())
            ngram[i] = iter->second;
        else
            ngram[i] = sunk_id;
    }

    for (unsigned int i = 0; i < context[1]; i++) {
        auto iter = target_vocab->find(*target[i]);

        if (iter != target_vocab->end())
            ngram[context[0] + i] = iter->second;
        else
            ngram[context[0] + i] = tunk_id;
    }

    auto iter = output_vocab->find(*label);

    if (iter != output_vocab->end())
        ngram[order - 1] = iter->second;
    else
        ngram[order - 1] = output_vocab->get_id("<unk>");

    //for (unsigned int i = 0; i < order; i++) {
    //    std::cout << *input[i] << " ";
    //}
    //std::cout << std::endl;


    score = model->ngram_prob(ngram);

    //std::cout << score << std::endl;

    delete[] ngram;

    return floor_score(score);
}
