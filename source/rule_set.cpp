/* rule_set.cpp */
#include <limits>
#include <algorithm>
#include <rule_set.h>

rule_list::rule_list(size_type size)
{
    if (size != 0) {
        hypothesis_vector = new std::vector<hypothesis_list*>();
        hypothesis_vector->reserve(size);
    } else
        hypothesis_vector = nullptr;
    rule_vector = nullptr;
}

rule_list::~rule_list()
{
    if (hypothesis_vector != nullptr)
        delete hypothesis_vector;
}

void rule_list::add_hypothese_list(hypothesis_list* list)
{
    hypothesis_vector->push_back(list);
}

std::vector<const rule*>* rule_list::get_rule_vector()
{
    return rule_vector;
}

rule_list::hypothesis_list* rule_list::get_hypothesis_vector(unsigned int pos)
{
    return hypothesis_vector->at(pos);
}

rule_list::size_type rule_list::get_rule_nonterminal_num() const
{
    auto r = rule_vector->at(0);
    unsigned int terminal_num = r->get_terminal_num();
    unsigned int symbol_num = r->get_target_rule_body().size();
    unsigned int nonterminal_num = symbol_num - terminal_num;

    return nonterminal_num;
}

void rule_list::set_rule_vector(const vector_type* vec)
{
    rule_vector = const_cast<vector_type*>(vec);
}

rule_list::size_type rule_list::size() const
{
    return rule_vector->size();
}

const symbol* rule_list::get_start_symbol() const
{
    const rule* r = rule_vector->at(0);
    return r->get_start_symbol();
}

rule_set::rule_set()
{
}

rule_set::~rule_set()
{
    clear();
}

void rule_set::add_complete_rule(partial_rule* rule)
{
    auto& rule_vec = *rule->get_rule();
    auto r = rule_vec[0];
    unsigned int terminal_num = r->get_terminal_num();
    unsigned int symbol_num = r->get_target_rule_body().size();
    unsigned int nonterminal_num = symbol_num - terminal_num;
    rule_list* list = new rule_list(nonterminal_num);
    const partial_rule* p = rule;

    while (nonterminal_num && p) {
        const symbol* sym = p->get_node()->get_symbol();

        if (sym->is_terminal()) {
            p = p->get_parent();
            continue;
        }

        nonterminal_num--;
        auto hypo_list = p->get_hypothesis_list();

        list->add_hypothese_list(hypo_list);
        p = p->get_parent();
    }

    list->set_rule_vector(&rule_vec);
    applicable_rule_set.push_back(list);
}

void rule_set::clear()
{
    unsigned int i;
    unsigned int size = applicable_rule_set.size();

    for (i = 0; i < size; i++)
        delete applicable_rule_set[i];
    applicable_rule_set.clear();
}

rule_set::size_type rule_set::size() const
{
    return applicable_rule_set.size();
}

rule_list* rule_set::get_rule_list(size_type index) const
{
    return applicable_rule_set[index];
}
