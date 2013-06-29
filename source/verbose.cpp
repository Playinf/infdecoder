/* verbose.cpp */
#include <vector>
#include <iostream>
#include <beam.h>
#include <rule.h>
#include <verbose.h>
#include <hypothesis.h>

void print_beam(const beam* b)
{
    beam* beam_stack = const_cast<beam*>(b);
    std::vector<hypothesis*>* ordered_hypothesis_list;
    ordered_hypothesis_list = beam_stack->get_sorted_hypothesis_list();

    for (unsigned int i = 0; i < ordered_hypothesis_list->size(); i++) {
        hypothesis* hypo = ordered_hypothesis_list->at(i);
        print_hypothesis(hypo);
    }
}

void print_rule(const rule* r)
{
    const symbol* lhs = r->get_start_symbol();
    const std::vector<const symbol*>& rhs = r->get_target_rule_body();

    print_symbol(lhs);
    std::cout << " -> ";

    for (unsigned int i = 0; i < rhs.size(); i++) {
        const symbol* sym = rhs[i];
        print_symbol(sym);
        std::cout << " ";
    }
    std::cout << std::endl;
}

void print_symbol(const symbol* s)
{
    std::cout << *s->get_name();

    if (s->is_nonterminal())
        std::cout << "[NONTERMINAL]";
}

void print_hypothesis(const hypothesis* hypo)
{
    std::vector<const std::string*> out_str;

    hypo->output(out_str);

    for (unsigned int i = 0; i < out_str.size(); i++) {
        std::cout << *out_str[i] << " ";
    }

    std::cout << " ||| " << hypo->get_total_score();
    std::cout << " ||| " << hypo->get_heuristic_score();
    std::cout << " ||| " << hypo->get_score() << std::endl;
}

void hypothesis_track(const hypothesis* hypo)
{
    std::cout << "HYPOTHESIS: ";
    print_hypothesis(hypo);
    std::cout << "RULE: ";
    print_rule(hypo->get_rule());

    unsigned int size = hypo->previous_hypothesis_number();

    for (unsigned int i = 0; i < size; i++) {
        const hypothesis* h = hypo->get_previous_hypothesis(i);
        hypothesis_track(h);
    }
}
