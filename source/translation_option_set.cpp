/* translation_option_set.cpp */
#include <limits>
#include <algorithm>
#include <rule.h>
#include <symbol.h>
#include <rule.h>
#include <rule_tree.h>
#include <hypothesis.h>
#include <information.h>
#include <partial_rule.h>
#include <translation_option.h>

translation_option_set::translation_option_set()
{
    // do nothing
}

translation_option_set::~translation_option_set()
{
    clear();
}

translation_option* translation_option_set::get_translation_option(size_type i)
    const
{
    return applicable_rule_set[i];
}

translation_option_set::size_type translation_option_set::size() const
{
    return applicable_rule_set.size();
}

void translation_option_set::clear()
{
    unsigned int size = applicable_rule_set.size();

    for (unsigned int i = 0; i < size; i++)
        delete applicable_rule_set[i];
    applicable_rule_set.clear();
}

void translation_option_set::add_complete_rule(partial_rule* rule)
{
    auto& rule_vec = *rule->get_rule();
    auto r = rule_vec[0];
    unsigned int nonterminal_num = r->get_nonterminal_number();
    translation_option* option = new translation_option(nonterminal_num);
    const partial_rule* p = rule;

    while (nonterminal_num && p) {
        const symbol* sym = p->get_node()->get_symbol();

        if (sym->is_terminal()) {
            p = p->get_parent();
            continue;
        }

        nonterminal_num--;
        auto hypo_list = p->get_hypothesis_list();
        option->add_hypothesis_list(nonterminal_num, hypo_list);
        p = p->get_parent();
    }

    option->set_rule(rule);
    option->set_rule_vector(&rule_vec);
    applicable_rule_set.push_back(option);
}

void translation_option_set::set_information(information* info)
{
    unsigned int size = applicable_rule_set.size();

    for (unsigned int i = 0; i < size; i++)
        applicable_rule_set[i]->set_input(info);
}

void translation_option_set::set_span(unsigned int start, unsigned int end)
{
    unsigned int size = applicable_rule_set.size();

    for (unsigned int i = 0; i < size; i++)
        applicable_rule_set[i]->set_span(start, end);
}
