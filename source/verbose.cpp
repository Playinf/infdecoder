/* verbose.cpp */
#include <vector>
#include <sstream>
#include <iostream>
#include <beam.h>
#include <rule.h>
#include <symbol.h>
#include <verbose.h>
#include <cube_item.h>
#include <rule_tree.h>
#include <chart_cell.h>
#include <hypothesis.h>
#include <partial_rule.h>
#include <translation_option.h>

static void output_hypothesis(const hypothesis* hypo, std::string& out)
{
    std::stringstream string_stream;
    const rule* target_rule = hypo->get_rule();
    auto& rule_body = target_rule->get_target_rule_body();
    unsigned int size = rule_body.size();
    unsigned int nonterminal_index = 0;

    for (unsigned int i = 0; i < size; ++i) {
        const symbol* sym = rule_body[i];

        if (sym->is_terminal())
            string_stream << *sym->get_name() << " ";
        else {
            std::string str_rep;
            hypothesis *h = hypo->get_previous_hypothesis(nonterminal_index);
            ++nonterminal_index;
            output_hypothesis(h, str_rep);
            string_stream << str_rep;
        }
    }

    out = string_stream.str();
}

static void output_partial_rule(const partial_rule* r, std::string& out)
{
    std::stringstream string_stream;
    const rt_node* node = r->get_node();
    const symbol* src_sym = node->get_symbol(0);
    const symbol* tgt_sym = node->get_symbol(1);
    const partial_rule* parent_rule = r->get_parent();

    if (parent_rule != nullptr) {
        std::string str_rep;
        output_partial_rule(parent_rule, str_rep);
        string_stream << str_rep;
    }

    if (src_sym != nullptr)
        string_stream << *src_sym->get_name();

    if (tgt_sym != nullptr) {
        auto& span = r->get_span();
        string_stream<< "-" << *tgt_sym->get_name();
        string_stream << "[" << span.first << ", ";
        string_stream << span.second << "]";
    }

    out = string_stream.str();
}

void to_string(const beam* b, std::string& out)
{
    std::stringstream string_stream;
    beam* beam_stack = const_cast<beam*>(b);
    std::vector<hypothesis*>* ordered_hypothesis_list;
    ordered_hypothesis_list = beam_stack->get_sorted_hypothesis_list();
    unsigned int size = ordered_hypothesis_list->size();

    for (unsigned int i = 0; i < size; i++) {
        std::string str_rep;
        hypothesis* hypo = ordered_hypothesis_list->at(i);
        to_string(hypo, str_rep);
        string_stream << str_rep << std::endl;
    }

    out = string_stream.str();
}

void to_string(const rule* r, std::string& out)
{
    std::stringstream string_stream;
    const unsigned int tgt_lhs_idx = 1;
    const symbol* lhs = r->get_start_symbol(tgt_lhs_idx);
    const std::vector<const symbol*>& rhs = r->get_target_rule_body();
    unsigned int size = rhs.size();
    float score = r->get_heuristic_score();
    std::string str_rep;

    to_string(lhs, str_rep);
    string_stream << str_rep << " -> ";

    for (unsigned int i = 0; i < size; i++) {
        const symbol* sym = rhs[i];
        to_string(sym, str_rep);
        string_stream << str_rep << " ";
    }

    string_stream << " ||| " << score;

    out = string_stream.str();
}

void to_string(const symbol* s, std::string& out)
{
    std::stringstream string_stream;

    string_stream << *s->get_name();

    if (s->is_nonterminal())
        string_stream << "[NONTERMINAL]";

    out = string_stream.str();
}

void to_string(const cube_item* i, std::string& out)
{
    std::stringstream string_stream;
    const cube_item* item = i;
    unsigned int rpos = item->get_rule_position();
    unsigned int hypo_num = item->get_hypothesis_dimension();

    string_stream << rpos;

    for (unsigned int i = 0; i < hypo_num; i++) {
        unsigned int hpos = item->get_hypothesis_position(i);
        string_stream << ":" << hpos;
    }

    string_stream << " ||| " << item->get_score() << std::endl;

    out = string_stream.str();
}

void to_string(const chart_cell* c, std::string& out)
{
    std::stringstream string_stream;
    std::vector<hypothesis*> hypo_list;
    unsigned int size;

    c->get_all_hypothesis(&hypo_list);
    size = hypo_list.size();

    for (unsigned int i = 0; i < size; i++) {
        hypothesis* h = hypo_list[i];
        std::string str_rep;
        string_stream << i << " ||| ";
        to_string(h, str_rep);
        string_stream << str_rep << std::endl;
    }

    out = string_stream.str();
}

void to_string(const hypothesis* h, std::string& out)
{
    std::stringstream string_stream;
    unsigned int size = h->get_feature_number();
    std::string str_rep;

    output_hypothesis(h, str_rep);
    string_stream << str_rep << " ||| ";

    for (unsigned int i = 0; i < size; i++) {
        const feature* f = h->get_feature(i);
        string_stream << f->get_score() << " ";
    }

    string_stream << " ||| " << h->get_total_score();
    out = string_stream.str();
}

void to_string(const partial_rule* r, std::string& out)
{
    std::stringstream string_stream;
    std::string str_rep;

    output_partial_rule(r, str_rep);
    string_stream << str_rep;
    out = string_stream.str();
}

void to_string(const translation_option* o, std::string& out)
{
    std::stringstream string_stream;
    auto vec = o->get_rule_vector();
    unsigned int size = vec->size();

    for (unsigned int i = 0; i < size; i++) {
        const rule* r = vec->at(i);
        std::string str_rep;

        to_string(r, str_rep);
        string_stream << str_rep << std::endl;
    }

    out = string_stream.str();
}

void to_string(const translation_option_set* s, std::string& out)
{
    std::stringstream string_stream;
    std::string str_rep;
    unsigned int size = s->size();

    for (unsigned int i = 0; i < size; i++) {
        translation_option* opt = s->get_translation_option(i);
        to_string(opt, str_rep);
        string_stream << str_rep;
    }

    out = string_stream.str();
}

void print_beam(const beam* b)
{
    std::string str_rep;

    to_string(b, str_rep);
    std::cout << str_rep << std::endl;
}

void print_rule(const rule* r)
{
    std::string str_rep;

    to_string(r, str_rep);
    std::cout << str_rep << std::endl;
}

void print_symbol(const symbol* s)
{
    std::string str_rep;

    to_string(s, str_rep);
    std::cout << str_rep;
}

void print_cube_item(const cube_item* i)
{
    std::string str_rep;

    to_string(i, str_rep);
    std::cout << str_rep;
}

void print_chart_cell(const chart_cell* c)
{
    std::string str_rep;

    to_string(c, str_rep);
    std::cout << str_rep;
}

void print_hypothesis(const hypothesis* h)
{
    std::string str_rep;

    to_string(h, str_rep);
    std::cout << str_rep << std::endl;
}

void print_partial_rule(const partial_rule* r)
{
    std::string str_rep;

    std::cout << r->get_length() << ":";
    to_string(r, str_rep);
    std::cout << str_rep << std::endl;
}

void print_translation_option(const translation_option* o)
{
    std::string str_rep;

    to_string(o, str_rep);
    std::cout << str_rep;
}

void print_translation_option_set(const translation_option_set* s)
{
    std::string str_rep;

    to_string(s, str_rep);
    std::cout << str_rep;
}

void hypothesis_track(const hypothesis* h)
{
    std::stringstream string_stream;
    std::string str_rep;

    hypothesis_track(h, str_rep);
    std::cout << str_rep;
}

void hypothesis_track(const hypothesis* hypo, std::string& out)
{
    std::stringstream string_stream;
    std::string str_rep;

    string_stream << "HYPOTHESIS: ";
    to_string(hypo, str_rep);
    string_stream << str_rep << std::endl;
    string_stream << "RULE: ";
    to_string(hypo->get_rule(), str_rep);
    string_stream << str_rep << std::endl;

    unsigned int size = hypo->get_previous_hypothesis_number();

    for (unsigned int i = 0; i < size; i++) {
        const hypothesis* h = hypo->get_previous_hypothesis(i);
        hypothesis_track(h, str_rep);
        string_stream << str_rep;
    }

    out = string_stream.str();
}
