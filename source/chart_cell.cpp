/* chart_cell.cpp */
#include <string>
#include <beam.h>
#include <cube.h>
#include <config.h>
#include <symbol.h>
#include <rule_set.h>
#include <chart_cell.h>
#include <hypothesis.h>

chart_cell::chart_cell()
{
}

chart_cell::~chart_cell()
{
    for (auto& b : nonterminal_beam_set) {
        delete b.second;
    }
}

void chart_cell::add_hypothesis(hypothesis* hypo)
{
    const symbol* source_start_symbol = hypo->get_start_symbol(0);
    const symbol* target_start_symbol = hypo->get_start_symbol(1);
    auto result = nonterminal_beam_set.find(target_start_symbol);
    configuration* config = configuration::get_instance();
    beam* nonterminal_beam;

    if (result == nonterminal_beam_set.end()) {
        const std::string& sym_name = *target_start_symbol->get_name();
        unsigned int beam_size = config->get_beam_size(sym_name);
        float beam_threshold = config->get_beam_threshold(sym_name);

        nonterminal_beam = new beam(beam_size, beam_threshold);
        nonterminal_beam_set[target_start_symbol] = nonterminal_beam;
    } else {
        nonterminal_beam = result->second;
    }

    nonterminal_beam->insert_hypothesis(hypo);

    source_start_symbol_set.insert(source_start_symbol);
    target_start_symbol_set.insert(target_start_symbol);
}

const chart_cell::symbol_set& chart_cell::get_source_start_symbol_set() const
{
    return source_start_symbol_set;
}

const chart_cell::symbol_set& chart_cell::get_target_start_symbol_set() const
{
    return target_start_symbol_set;
}

void chart_cell::get_all_hypothesis(hypothesis_list* hypo_list) const
{
    for (auto& b : nonterminal_beam_set) {
        beam* nonterm_beam = b.second;
        auto hlist = nonterm_beam->get_sorted_hypothesis_list();
        unsigned int size;

        if (hlist == nullptr)
            continue;

        size = hlist->size();

        for (unsigned int i = 0; i < size; i++) {
            hypothesis* hypo = hlist->at(i);
            hypo_list->push_back(hypo);
        }
    }
}

chart_cell::hypothesis_list* chart_cell::get_hypothesis_list(const symbol* lhs)
{
    if (lhs->is_terminal())
        return nullptr;

    auto result = nonterminal_beam_set.find(lhs);

    if (result == nonterminal_beam_set.end())
        return nullptr;

    beam* nonterminal_beam = result->second;

    return nonterminal_beam->get_sorted_hypothesis_list();
}

void chart_cell::decode(rule_set* s, unsigned int limit)
{
    unsigned int size = s->size();
    cube_queue queue;

    for (unsigned int i = 0; i < size; i++) {
        rule_list* list = s->get_rule_list(i);
        cube* rule_cube = new cube(list);

        queue.insert(rule_cube);
    }

    for (unsigned int i = 0; i < limit; i++) {
        if (queue.empty())
            break;

        hypothesis* hypo = const_cast<hypothesis*>(queue.pop());
        add_hypothesis(hypo);
    }
}

void chart_cell::sort()
{
    for (auto& b : nonterminal_beam_set) {
        b.second->sort();
    }
}
