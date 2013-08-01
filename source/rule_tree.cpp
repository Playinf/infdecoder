/* rule_tree.cpp */
#include <symbol.h>
#include <rule_tree.h>

rule_tree::rule_tree()
{
    const unsigned int default_id = 0;

    id = default_id;
    node_num = 0;
    rule_num = 0;
    rule_heuristic_function = nullptr;
}

rule_tree::~rule_tree()
{
    /* do nothing */
}

rule_tree::size_type rule_tree::get_rule_number() const
{
    return rule_num;
}

rule_tree::size_type rule_tree::get_node_number() const
{
    return node_num;
}

const rule_tree::node_type* rule_tree::get_root() const
{
    return &root;
}

unsigned int rule_tree::get_id() const
{
    return id;
}

void rule_tree::sort()
{
    rt_node::sort(&root, rule_heuristic_function);
}

void rule_tree::sort(unsigned int limit)
{
    if (!limit)
        rt_node::sort(&root, rule_heuristic_function);
    else
        rt_node::sort(&root, rule_heuristic_function, limit);
}

void rule_tree::prune(unsigned int limit)
{
    if (!limit)
        return;

    rule_num = rt_node::prune(&root, limit);
}

void rule_tree::set_id(unsigned int id)
{
    this->id = id;
}

void rule_tree::set_heuristic_function(heuristic_function func)
{
    rule_heuristic_function = func;
}

rt_node* rule_tree::insert_node(rt_node* parent, const symbol* sym)
{
    auto result = parent->insert_child(sym);

    if (result.second)
        node_num += 1;

    return result.first;
}

rt_node* rule_tree::insert_node(rt_node* p, const symbol* src, const symbol* tgt)
{
    auto result = p->insert_child(src, tgt);

    if (result.second)
        node_num += 1;

    return result.first;
}

rt_node* rule_tree::find_child(const rt_node* parent, const symbol* sym)
{
    return parent->find_child(sym);
}

rt_node* rule_tree::find_child(const rt_node* parent, const symbol* src,
    const symbol* tgt)
{
    return parent->find_child(src, tgt);
}

void rule_tree::insert_rule(rt_node* node, rule* r)
{
    rule_num += 1;
    r->set_rule_tree_id(id);
    node->insert_rule(r);
}
