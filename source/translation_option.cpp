/* translation_option.cpp */
#include <vector>
#include <utility>
#include <rule.h>
#include <symbol.h>
#include <hypothesis.h>
#include <information.h>
#include <translation_option.h>

translation_option::translation_option(size_type size)
{
    if (size != 0) {
        hypothesis_vector = new std::vector<hypothesis_list*>();
        hypothesis_vector->resize(size);
    } else
        hypothesis_vector = nullptr;

    input = nullptr;
    source_rule = nullptr;
    rule_vector = nullptr;
}

translation_option::~translation_option()
{
    if (hypothesis_vector != nullptr)
        delete hypothesis_vector;
}

translation_option::size_type translation_option::size() const
{
    return rule_vector->size();
}

information* translation_option::get_input() const
{
    return input;
}

const partial_rule* translation_option::get_rule() const
{
    return source_rule;
}

const symbol* translation_option::get_start_symbol() const
{
    // target nonterminal
    const unsigned int index = 1;
    const rule* r = rule_vector->at(0);

    return r->get_start_symbol(index);
}

unsigned int translation_option::get_rule_nonterminal_number() const
{
    auto r = rule_vector->at(0);

    return r->get_nonterminal_number();
}

std::vector<const rule*>* translation_option::get_rule_vector() const
{
    return rule_vector;
}

std::vector<hypothesis*>* translation_option::get_hypothesis_list(size_type i)
    const
{
    return hypothesis_vector->at(i);
}

std::pair<unsigned int, unsigned int> translation_option::get_span() const
{
    return span;
}

void translation_option::set_rule(partial_rule* r)
{
    source_rule = r;
}

void translation_option::set_input(information* input)
{
    this->input = input;
}

void translation_option::set_rule_vector(const vector_type* vec)
{
    rule_vector = const_cast<vector_type*>(vec);
}

void translation_option::set_span(unsigned int start, unsigned int end)
{
    span.first = start;
    span.second = end;
}

void translation_option::add_hypothesis_list(unsigned int index,
    hypothesis_list* list)
{
    hypothesis_list** data = hypothesis_vector->data();

    data[index] = list;
}
