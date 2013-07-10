/* rule_tree.cpp */
#include <string>
#include <algorithm>
#include <symbol.h>
#include <rule_tree.h>

/* rt_node */

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

bool rt_node::operator==(const rt_node& node) const
{
    if (link[0] == node.link[0] && link[1] == node.link[1])
        return true;

    return false;
}

bool rt_node::is_leaf() const
{
    return child_nodes == nullptr;
}

const symbol* rt_node::get_symbol() const
{
    return link[0];
}

const symbol* rt_node::get_symbol(int index) const
{
    return link[index];
}

const rt_node::vector_type* rt_node::get_rules() const
{
    return rule_vector;
}

void rt_node::sort()
{
    auto begin = rule_vector->begin();
    auto end = rule_vector->end();

    //std::sort(begin, end, rule_less());
}

void rt_node::sort(unsigned int limit)
{
    auto begin = rule_vector->begin();
    auto end = rule_vector->end();

    /* using partial_sort instead of sort */
    //std::partial_sort(begin, end, rule_less());
}

rt_node::size_type rt_node::prune(unsigned int limit)
{
    size_type size = rule_vector->size();

    if (size > limit) {
        rule_vector->resize(limit);
        size = limit;
    }

    return size;
}

void rt_node::insert_rule(const rule* r)
{
    if (rule_vector == nullptr)
        rule_vector = new std::vector<const rule*>;

    rule_vector->push_back(r);
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

rt_node* rt_node::find_child(const symbol* src, const symbol* tgt) const
{
    rt_node node(src, tgt);

    if (child_nodes != nullptr) {
        auto end = child_nodes->end();
        auto result = child_nodes->find(node);

        if (result != end)
            return const_cast<rt_node*>(&(*result));
    }

    return nullptr;
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

std::pair<rt_node*, bool> rt_node::insert_child(const symbol* src,
    const symbol* tgt)
{
    rt_node node(src, tgt);

    if (child_nodes == nullptr)
        child_nodes = new set_type;

    auto result = child_nodes->insert(node);
    rt_node* node_ptr = const_cast<rt_node*>(&(*result.first));

    return std::pair<rt_node*, bool>(node_ptr, result.second);
}

void rt_node::sort(rt_node* node)
{
    auto rules = node->rule_vector;
    auto child = node->child_nodes;

    if (rules != nullptr && rules->size() > 1)
        node->sort();

    if (child == nullptr)
        return;

    for (auto iter = child->begin(); iter != child->end(); ++iter) {
        rt_node* n = const_cast<rt_node*>(&(*iter));
        sort(n);
    }
}

void rt_node::sort(rt_node* node, unsigned int limit)
{
    auto rules = node->rule_vector;
    auto child = node->child_nodes;

    if (rules != nullptr && rules->size() > 1)
        node->sort(limit);

    if (child == nullptr)
        return;

    for (auto iter = child->begin(); iter != child->end(); ++iter) {
        rt_node* n = const_cast<rt_node*>(&(*iter));
        sort(n, limit);
    }
}

rt_node::size_type rt_node::prune(rt_node* node, unsigned int limit)
{
    size_type num = 0;
    auto rules = node->rule_vector;
    auto child = node->child_nodes;

    num += node->prune(limit);

    if (child == nullptr)
        return num;

    auto begin = child->begin();
    auto end = child->end();
    auto iter = begin;

    while (iter != end) {
        rt_node* n = const_cast<rt_node*>(&(*iter));
        num += prune(n, limit);
        ++iter;
    }

    return num;
}

/* rule_tree */

rule_tree::rule_tree()
{
    node_num = 0;
    rule_num = 0;
    id = 0;
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
    rt_node::sort(&root);
}

void rule_tree::sort(unsigned int limit)
{
    if (!limit)
        rt_node::sort(&root);
    else
        rt_node::sort(&root, limit);
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
    return node->insert_rule(r);
}
