/*
 * rule_tree.h
 *
 *
 * author: Playinf
 * email: playinf@stu.xmu.edu.cn
 *
 */
#ifndef __RULE_TREE_H__
#define __RULE_TREE_H__

#include <vector>
#include <utility>
#include <unordered_set>
#include <rule.h>
#include <functional.h>

class symbol;

class rt_node {
public:
    typedef std::vector<const rule*> vector_type;
    typedef std::unordered_set<rt_node, rt_node_hash> set_type;

    rt_node();
    rt_node(const symbol* sym);
    rt_node(const symbol* src, const symbol* tgt);
    ~rt_node();

    rt_node& operator=(const rt_node& n) = delete;

    const symbol* get_source_symbol() const;
    const symbol* get_target_symbol() const;
    const symbol* get_symbol(int index) const;
    const vector_type* get_rules() const;
    bool is_leaf() const;
    bool operator==(const rt_node& node) const;
    void insert_rule(const rule* r);
    std::pair<rt_node*, bool> insert_child(const symbol* sym);
    rt_node* find_child(const symbol* sym) const;
    rt_node* find_child(const symbol* src, const symbol* tgt);
    static void sort(rt_node* node, unsigned limit);
    void sort(unsigned int limit);
private:
    const symbol* link[2];
    vector_type* rule_vector;
    set_type* child_nodes;
};

class rule_tree {
public:
    typedef size_t size_type;
    typedef rt_node node_type;

    rule_tree();
    ~rule_tree();

    rule_tree(const rule_tree& t) = delete;
    rule_tree& operator=(const rule_tree& t) = delete;

    const node_type* get_root() const;
    rt_node* insert_node(rt_node* parent, const symbol* sym);
    rt_node* insert_node(rt_node* parent, const symbol* src, const symbol* tgt);
    rt_node* find_child(const rt_node* parent, const symbol* sym);
    void insert_rule(rt_node* node, rule* r);
    void sort();
private:
    node_type root;
    size_type node_num;
    size_type rule_num;
};

#endif /* __RULE_TREE_H__ */
