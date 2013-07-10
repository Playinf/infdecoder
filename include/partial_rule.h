/*
 * partial_rule.h
 *
 *
 * author: Playinf
 * email: playinf@stu.xmu.edu.cn
 *
 */
#ifndef __PARTIAL_RULE_H__
#define __PARTIAL_RULE_H__

#include <vector>
#include <string>

class rule;
class rule_tree;
class hypothesis;

class partial_rule {
public:
    typedef unsigned int size_type;

    partial_rule();
    partial_rule(const rt_node* n, partial_rule* parent, size_type len);
    ~partial_rule();

    partial_rule(const partial_rule& p) = delete;
    partial_rule& operator=(const partial_rule& p) = delete;

    unsigned int get_length() const;
    const rt_node* get_node() const;
    const partial_rule* get_parent() const;
    const std::vector<const rule*>* get_rule() const;
    std::vector<hypothesis*>* get_hypothesis_list() const;

    bool is_expandable() const;
    void set_hypothesis_list(std::vector<hypothesis*>* hlist);
private:
    const rt_node* node;
    const partial_rule* parent;
    std::vector<hypothesis*>* nonterminal_hypothesis_list;
    unsigned int length;
};

class partial_rule_set {
public:
    typedef unsigned int size_type;
    typedef std::vector<partial_rule*> list_type;
    typedef std::vector<list_type> set_type;

    partial_rule_set(size_type size);
    ~partial_rule_set();

    partial_rule_set(const partial_rule_set& p) = delete;
    partial_rule_set& operator=(const partial_rule_set& p) = delete;

    const list_type* get_partial_rule_list(unsigned int end_pos) const;
    const list_type* get_expandable_rule_list() const;

    void insert_partial_rule(partial_rule* r);
private:
    set_type partial_rules;
    list_type expandable_rule_list;
};

#endif /* __PARTIAL_RULE_H__ */
