/* partial_rule.cpp */
#include <symbol.h>
#include <partial_rule.h>

partial_rule::partial_rule()
{
    node = nullptr;
    length = 0;
    symbol_hypothesis_list = nullptr;
    parent = nullptr;
}

partial_rule::partial_rule(const rt_node* node, partial_rule* parent,
    unsigned int len)
{
    this->node = node;
    length = len;
    symbol_hypothesis_list = nullptr;
    this->parent = parent;
}

partial_rule::~partial_rule()
{
    // do nothing
}

void partial_rule::set_hypothesis_list(std::vector<hypothesis*>* hlist)
{
    symbol_hypothesis_list = hlist;
}

std::vector<hypothesis*>* partial_rule::get_hypothesis_list() const
{
    return symbol_hypothesis_list;
}

const rt_node* partial_rule::get_node() const
{
    return node;
}

const partial_rule* partial_rule::get_parent() const
{
    return parent;
}

const std::vector<const rule*>* partial_rule::get_rule() const
{
    if (node) {
        return node->get_rules();
    }

    return nullptr;
}

void partial_rule::output_partial_rule(std::string& str) const
{
    const symbol* sym = node->get_symbol();

    if (parent != nullptr)
        parent->output_partial_rule(str);

    if (sym != nullptr)
        str += *sym->get_name() + " ";
}

unsigned int partial_rule::get_length() const
{
    return length;
}

bool partial_rule::is_expandable() const
{
    if (node)
        return !node->is_leaf();

    return false;
}

partial_rule_set::partial_rule_set(size_type size) : partial_rules(size)
{
}

partial_rule_set::~partial_rule_set()
{
    auto size = partial_rules.size();

    for (unsigned int i = 0; i < size; i++) {
        auto& rule_list = partial_rules[i];
        auto list_size = rule_list.size();

        for (unsigned int j = 0; j < list_size; j++)
            delete rule_list[j];
    }
}

void partial_rule_set::insert_partial_rule(partial_rule* r)
{
    unsigned int pos = r->get_length();
    partial_rules[pos].push_back(r);

    if (r->is_expandable())
        expandable_rule_list.push_back(r);
}

const partial_rule_set::list_type*
    partial_rule_set::get_partial_rule_list(unsigned int end_pos) const
{
    return &partial_rules[end_pos];
}

const partial_rule_set::list_type*
    partial_rule_set::get_expandable_rule_list() const
{
    return &expandable_rule_list;
}
