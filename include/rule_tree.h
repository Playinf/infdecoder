/*
 * rule_tree.h
 * 
 * author: Playinf
 * playinf@stu.xmu.edu.cn
 *
 */
#ifndef __RULE_TREE_H__
#define __RULE_TREE_H__

#include <vector>
#include <utility>
#include <unordered_set>
#include <rule.h>
#include <symbol.h>

class rt_node;

struct rt_node_hash {
    size_t operator()(const rt_node& node) const;
};

class rt_node {
public:
    typedef std::vector<const rule*> vector_type;
    typedef std::unordered_set<rt_node, rt_node_hash> set_type;
    
    rt_node();
    rt_node(const symbol* sym);
    ~rt_node();

    const symbol* get_symbol() const;
    const vector_type* get_rules() const;
    bool is_leaf() const;
    bool operator==(const rt_node& node) const;

    static void set_rule_limit(unsigned int limit);
private:
    friend class rule_tree;

    void insert_rule(const rule* r);
    std::pair<rt_node*, bool> insert_child(const symbol* sym);
    rt_node* find_child(const symbol* sym) const;

    static void sort(rt_node* node);
    void sort();

    const symbol* link;
    vector_type* rule_vector;
    set_type* child_nodes;
    static unsigned int rule_limit;
};

class rule_tree {
public:
    typedef size_t size_type;
    typedef rt_node node_type;

    rule_tree();
    ~rule_tree();

    const node_type* get_root() const;    
    rt_node* insert_node(rt_node* parent, const symbol* sym);
    rt_node* find_child(const rt_node* parent, const symbol* sym);
    void insert_rule(rt_node* node, rule* r);
    void sort();
private:
    node_type root;
    size_type node_num;
    size_type rule_num;
};

#endif /* __RULE_TREE_H__ */
