/* rt_node.cpp */
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

void rt_node::sort(heuristic_function func)
{
    auto begin = rule_vector->begin();
    auto end = rule_vector->end();

    for (auto iter = begin; iter != end; ++iter) {
        rule* r = const_cast<rule*>(*iter);
        float score = func(r);

        r->set_heuristic_score(score);
    }

    std::sort(begin, end, rule_less());
}

void rt_node::sort(heuristic_function func, unsigned int limit)
{
    auto begin = rule_vector->begin();
    auto end = rule_vector->end();
    unsigned int size = rule_vector->size();
    auto mid = (size > limit) ? begin + limit : end;

    for (auto iter = begin; iter != end; ++iter) {
        rule* r = const_cast<rule*>(*iter);
        float score = func(r);

        r->set_heuristic_score(score);
    }

    /* using partial_sort instead of sort */
    std::partial_sort(begin, mid, end, rule_less());
}

rt_node::size_type rt_node::prune(unsigned int limit)
{
    size_type size = rule_vector->size();

    if (size <= limit)
        return size;

    for (unsigned int i = limit; i < size; i++)
        delete rule_vector->at(i);

    auto iter_begin = rule_vector->begin() + limit;
    auto iter_end = rule_vector->end();

    rule_vector->erase(iter_begin, iter_end);

    return limit;
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

void rt_node::sort(rt_node* node, heuristic_function func)
{
    auto rules = node->rule_vector;
    auto child = node->child_nodes;

    if (rules != nullptr && rules->size() > 1)
        node->sort(func);

    if (child == nullptr)
        return;

    for (auto iter = child->begin(); iter != child->end(); ++iter) {
        rt_node* n = const_cast<rt_node*>(&(*iter));
        sort(n, func);
    }
}

void rt_node::sort(rt_node* node, heuristic_function func, unsigned int limit)
{
    auto rules = node->rule_vector;
    auto child = node->child_nodes;

    if (rules != nullptr && rules->size() > 1)
        node->sort(func, limit);

    if (child == nullptr)
        return;

    for (auto iter = child->begin(); iter != child->end(); ++iter) {
        rt_node* n = const_cast<rt_node*>(&(*iter));
        sort(n, func, limit);
    }
}

rt_node::size_type rt_node::prune(rt_node* node, unsigned int limit)
{
    size_type num = 0;
    auto rules = node->rule_vector;
    auto child = node->child_nodes;

    if (rules != nullptr && rules->size() > limit)
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
