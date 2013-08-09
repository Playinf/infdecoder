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

typedef float (*heuristic_function)(const rule* r);

class rt_node {
public:
    typedef unsigned int size_type;
    typedef std::vector<const rule*> vector_type;
    typedef std::unordered_set<rt_node, rt_node_hash> set_type;

    rt_node();
    rt_node(const symbol* sym);
    rt_node(const symbol* src, const symbol* tgt);
    ~rt_node();

    rt_node& operator=(const rt_node& n) = delete;
    bool operator==(const rt_node& node) const;

    bool is_leaf() const;
    const symbol* get_symbol() const;
    const symbol* get_symbol(int index) const;
    const vector_type* get_rules() const;

    void insert_rule(const rule* r);
    void sort(heuristic_function func);
    size_type sort(heuristic_function func, unsigned int limit);
    rt_node* find_child(const symbol* sym) const;
    rt_node* find_child(const symbol* src, const symbol* tgt) const;
    std::pair<rt_node*, bool> insert_child(const symbol* sym);
    std::pair<rt_node*, bool> insert_child(const symbol* s, const symbol* t);

    static void sort(rt_node* n, heuristic_function f);
    static size_type sort(rt_node* n, heuristic_function f, unsigned int lim);
private:
    const symbol* link[2];
    vector_type* rule_vector;
    set_type* child_nodes;
};

class rule_tree {
public:
    typedef unsigned int size_type;
    typedef rt_node node_type;

    rule_tree();
    ~rule_tree();

    rule_tree(const rule_tree& t) = delete;
    rule_tree& operator=(const rule_tree& t) = delete;

    unsigned int get_id() const;
    size_type get_rule_number() const;
    size_type get_node_number() const;
    const node_type* get_root() const;

    void set_id(unsigned int id);
    void sort_and_prune(unsigned int limit);
    void set_heuristic_function(heuristic_function func);
    rt_node* insert_node(rt_node* parent, const symbol* sym);
    rt_node* insert_node(rt_node* parent, const symbol* src, const symbol* tgt);
    rt_node* find_child(const rt_node* parent, const symbol* sym);
    rt_node* find_child(const rt_node* p, const symbol* src, const symbol* tgt);
    void insert_rule(rt_node* node, rule* r);
private:
    node_type root;
    unsigned int id;
    size_type node_num;
    size_type rule_num;
    heuristic_function rule_heuristic_function;
};

#endif /* __RULE_TREE_H__ */
