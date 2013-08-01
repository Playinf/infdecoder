/* rule.cpp */
#include <vector>
#include <rule.h>
#include <symbol.h>
#include <language_model.h>

rule::rule(size_type size)
{
    id = 0;
    score = nullptr;
    heuristic_score = 0.0f;
    start_symbol[0] = nullptr;
    start_symbol[1] = nullptr;
    target_terminal_number = 0;
    nonterminal_map = nullptr;
    target_rule_body.reserve(size);
}

rule::~rule()
{
    delete[] score;
    delete[] nonterminal_map;
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

unsigned int rule::get_nonterminal_map(unsigned int src) const
{
    return nonterminal_map[src];
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

    score = new float[feature_number];

    for (unsigned int i = 0; i < feature_number; i++)
        score[i] = vec[i];
}

void rule::add_align(unsigned int src, unsigned int tgt)
{
    unsigned int nonterm_num = target_rule_body.size() - target_terminal_number;

    if (nonterminal_map == nullptr)
        nonterminal_map = new unsigned int[nonterm_num];

    nonterminal_map[src] = tgt;
}

void rule::set_rule_tree_id(unsigned int id)
{
    this->id = id;
}

void rule::set_start_symbol(const symbol* src, const symbol* tgt)
{
    start_symbol[0] = src;
    start_symbol[1] = tgt;
}
