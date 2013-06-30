/* rule_tree.cpp */
#include <string>
#include <algorithm>
#include <symbol.h>
#include <rule_tree.h>

rt_node::rt_node()
{
    link[0] = nullptr;
    link[1] = nullptr;
    rule_vector = nullptr;
    child_nodes = nullptr;
}

rt_node::rt_node(const symbol* sym)
{
    link[0] = sym;
    link[1] = nullptr;
    rule_vector = nullptr;
    child_nodes = nullptr;
}

rt_node::rt_node(const symbol* src, const symbol* tgt)
{
    link[0] = src;
    link[1] = tgt;
    rule_vector = nullptr;
    child_nodes = nullptr;
}

rt_node::~rt_node()
{
    if (rule_vector != nullptr) {
        unsigned int size = rule_vector->size();

        // delete the allocated rule
        for (unsigned int i = 0; i < size; i++)
            delete rule_vector->at(i);
        delete rule_vector;
    }

    if (child_nodes != nullptr)
        delete child_nodes;
}

void rt_node::insert_rule(const rule* r)
{
    if (rule_vector == nullptr)
        rule_vector = new std::vector<const rule*>;

    rule_vector->push_back(r);
}

std::pair<rt_node*, bool> rt_node::insert_child(const symbol* sym)
{
    rt_node node(sym);

    if (child_nodes == nullptr)
        child_nodes = new set_type;

    auto result = child_nodes->insert(node);
    rt_node* node_ptr = const_cast<rt_node*>(&(*result.first));

    return std::pair<rt_node*, bool>(node_ptr, result.second);
}

rt_node* rt_node::find_child(const symbol* sym) const
{
    rt_node node(sym);

    if (child_nodes != nullptr) {
        auto end = child_nodes->end();
        auto result = child_nodes->find(node);

        if (result != end)
            return const_cast<rt_node*>(&(*result));
    }

    return nullptr;
}

const symbol* rt_node::get_symbol(int index) const
{
    return link[index];
}

const rt_node::vector_type* rt_node::get_rules() const
{
    return rule_vector;
}

bool rt_node::is_leaf() const
{
    return child_nodes == nullptr;
}

bool rt_node::operator==(const rt_node& node) const
{
    if (link[0] == node.link[0] && link[1] == node.link[1])
        return true;

    return false;
}

void rt_node::sort(rt_node* node, unsigned int limit)
{
    auto rules = node->rule_vector;
    auto child = node->child_nodes;

    if (rules != nullptr && rules->size() > 1)
        node->sort(limit);

    if (child == nullptr)
        return;

    auto begin = child->begin();
    auto end = child->end();
    auto iter = begin;

    while (iter != end) {
        rt_node* n = const_cast<rt_node*>(&(*iter));
        sort(n, limit);
        ++iter;
    }
}

void rt_node::sort(unsigned int limit)
{
    auto begin = rule_vector->begin();
    auto end = rule_vector->end();

    std::sort(begin, end, rule_less());

    if (rule_vector->size() > limit)
        rule_vector->resize(limit);
}

rule_tree::rule_tree()
{
    node_num = 0;
    rule_num = 0;
}

rule_tree::~rule_tree()
{
    // do nothing
}

const rule_tree::node_type* rule_tree::get_root() const
{
    return &root;
}

rt_node* rule_tree::insert_node(rt_node* parent, const symbol* sym)
{
    auto result = parent->insert_child(sym);

    if (result.second)
        node_num += 1;

    return result.first;
}

rt_node* rule_tree::find_child(const rt_node* parent, const symbol* sym)
{
    return parent->find_child(sym);
}

void rule_tree::insert_rule(rt_node* node, rule* r)
{
    rule_num += 1;
    return node->insert_rule(r);
}

void rule_tree::sort()
{
    rt_node::sort(&root, 20);
}
