/* verbose.cpp */
#include <vector>
#include <iostream>
#include <beam.h>
#include <rule.h>
#include <symbol.h>
#include <verbose.h>
#include <rule_set.h>
#include <rule_tree.h>
#include <chart_cell.h>
#include <hypothesis.h>
#include <partial_rule.h>

void print_beam(const beam* b)
{
    beam* beam_stack = const_cast<beam*>(b);
    std::vector<hypothesis*>* ordered_hypothesis_list;
    ordered_hypothesis_list = beam_stack->get_sorted_hypothesis_list();
    unsigned int size = ordered_hypothesis_list->size();

    for (unsigned int i = 0; i < size; i++) {
        hypothesis* hypo = ordered_hypothesis_list->at(i);
        print_hypothesis(hypo);
    }
}

void print_rule(const rule* r)
{
    const unsigned int tgt_lhs_idx = 1;
    const symbol* lhs = r->get_start_symbol(tgt_lhs_idx);
    const std::vector<const symbol*>& rhs = r->get_target_rule_body();
    unsigned int size = rhs.size();
    float score = r->get_heuristic_score();

    print_symbol(lhs);
    std::cout << " -> ";

    for (unsigned int i = 0; i < size; i++) {
        const symbol* sym = rhs[i];
        print_symbol(sym);
        std::cout << " ";
    }

    std::cout << " ||| " << score;

    std::cout << std::endl;
}

void print_symbol(const symbol* s)
{
    std::cout << *s->get_name();

    if (s->is_nonterminal())
        std::cout << "[NONTERMINAL]";
}

void print_rule_set(const rule_set* s)
{
    unsigned int size = s->size();

    for (unsigned int i = 0; i < size; i++) {
        rule_list* l = s->get_rule_list(i);

        print_rule_list(l);
    }
}

void print_rule_list(const rule_list* l)
{
    auto vec = l->get_rule_vector();
    unsigned int size = vec->size();

    for (unsigned int i = 0; i < size; i++) {
        const rule* r = vec->at(i);

        print_rule(r);
    }
}

void print_chart_cell(const chart_cell* c)
{
    std::vector<hypothesis*> hypo_list;
    unsigned int size;

    c->get_all_hypothesis(&hypo_list);
    size = hypo_list.size();

    for (unsigned int i = 0; i < size; i++) {
        hypothesis* h = hypo_list[i];
        std::cout << i << " ||| ";
        print_hypothesis(h);
    }
}

void print_partial_translation(const hypothesis* hypo)
{
    const rule* target_rule = hypo->get_rule();
    auto& rule_body = target_rule->get_target_rule_body();
    unsigned int size = rule_body.size();
    unsigned int nonterminal_index = 0;

    for (unsigned int i = 0; i < size; ++i) {
        const symbol* sym = rule_body[i];

        if (sym->is_terminal())
            std::cout << *sym->get_name() << " ";
        else {
            hypothesis *h = hypo->get_previous_hypothesis(nonterminal_index);
            ++nonterminal_index;
            print_partial_translation(h);
        }
    }
}

void print_hypothesis(const hypothesis* hypo)
{
    unsigned int size = hypo->get_feature_number();

    print_partial_translation(hypo);

    std::cout << " ||| ";

    for (unsigned int i = 0; i < size; i++) {
        const feature* fea = hypo->get_feature(i);
        std::cout << fea->get_score() << " ";
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

    unsigned int size = hypo->get_previous_hypothesis_number();

    for (unsigned int i = 0; i < size; i++) {
        const hypothesis* h = hypo->get_previous_hypothesis(i);
        hypothesis_track(h);
    }
}

static void print_partial_rule(const partial_rule* r, bool end)
{
    const rt_node* node = r->get_node();
    const symbol* src_sym = node->get_symbol(0);
    const symbol* tgt_sym = node->get_symbol(1);
    const partial_rule* parent_rule = r->get_parent();

    if (parent_rule != nullptr)
        print_partial_rule(parent_rule, false);

    if (src_sym != nullptr)
        std::cout << *src_sym->get_name();

    if (tgt_sym != nullptr) {
        auto& span = r->get_span();
        std::cout << "-" << *tgt_sym->get_name();
        std::cout << "[" << span.first << ", ";
        std::cout << span.second << "]";
    }

    if (end)
        std::cout << std::endl;
    else
        std::cout << " ";
}

void print_partial_rule(const partial_rule* r)
{
    std::cout << r->get_length() << ":";
    print_partial_rule(r, true);
}
