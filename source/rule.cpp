/* rule.cpp */
#include <vector>
#include <rule.h>
#include <symbol.h>
#include <alignment.h>

rule::rule(size_type size)
{
    id = 0;
    score = nullptr;
    heuristic_score = 0.0f;
    start_symbol[0] = nullptr;
    start_symbol[1] = nullptr;
    target_terminal_number = 0;
    symbol_alignment = nullptr;
    target_rule_body.reserve(size);
}

rule::~rule()
{
    delete[] score;
}

unsigned int rule::get_id() const
{
    return id;
}

float rule::get_heuristic_score() const
{
    return heuristic_score;
}

float rule::get_score(unsigned int index) const
{
    return score[index];
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

const alignment* rule::get_alignment() const
{
    return symbol_alignment;
}

unsigned int rule::get_nonterminal_number() const
{
    return target_rule_body.size() - target_terminal_number;
}

void rule::set_id(unsigned int id)
{
    this->id = id;
}

void rule::add_symbol(const symbol* sym)
{
    target_rule_body.push_back(sym);

    if (sym->is_terminal())
        target_terminal_number++;
}

void rule::set_heuristic_score(float score)
{
    heuristic_score = score;
}

void rule::set_score(std::vector<float>& vec)
{
    unsigned int feature_number = vec.size();

    if (feature_number == 0)
        return;

    score = new float[feature_number];

    for (unsigned int i = 0; i < feature_number; i++)
        score[i] = vec[i];
}

void rule::set_alignment(const alignment* align)
{
    symbol_alignment = align;
}

void rule::set_start_symbol(const symbol* src, const symbol* tgt)
{
    start_symbol[0] = src;
    start_symbol[1] = tgt;
}
