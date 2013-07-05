/* rule.cpp */
#include <vector>
#include <rule.h>
#include <symbol.h>
#include <language_model.h>

rule::rule(size_type size)
{
    score = nullptr;
    feature_number = 0;
    start_symbol[0] = nullptr;
    start_symbol[1] = nullptr;
    target_terminal_number = 0;
    target_rule_body.reserve(size);
}

rule::~rule()
{
    delete[] score;
}

float rule::get_score(unsigned int index) const
{
    return score[index];
}

unsigned int rule::get_feature_number() const
{
    return feature_number;
}

rule::size_type rule::get_terminal_number() const
{
    return target_terminal_number;
}

const symbol* rule::get_start_symbol(int index) const
{
    return start_symbol[index];
}

const std::vector<const symbol*>& rule::get_target_rule_body() const
{
    return target_rule_body;
}

/*
void rule::evaluate_score(const language_model* lm, const weight_vector& weight)
{
    unsigned int size = target_rule_body.size();
    const float penalty = -1.0f;
    float terminal_num_weight = weight[4];

    for (unsigned int i = 0; i < size; i++) {
        const symbol* sym = target_rule_body[i];

        score += lm->word_probability(*sym->get_name());
    }

    size = feature_set.size();

    for (unsigned int i = 0; i < size; i++) {
        const feature& fea = feature_set[i];
        float w = weight[i];

        score += w * fea.get_score();
    }

    score += terminal_num_weight * penalty * target_terminal_number;
}
*/

void rule::add_symbol(const symbol* sym)
{
    target_rule_body.push_back(sym);

    if (sym->is_terminal())
        target_terminal_number++;
}

void rule::set_score(std::vector<float>& vec)
{
    feature_number = vec.size();

    score = new float[feature_number];

    for (unsigned int i = 0; i < vec.size(); i++)
        score[i] = vec[i];
}

void rule::set_start_symbol(const symbol* src, const symbol* tgt)
{
    start_symbol[0] = src;
    start_symbol[1] = tgt;
}
