/*
 * rule_set.h
 *
 *
 * author: Playinf
 * playinf@stu.xmu.edu.cn
 *
 */
#ifndef __RULE_SET_H__
#define __RULE_SET_H__

#include <vector>
#include <rule.h>
#include <symbol.h>
#include <hypothesis.h>
#include <partial_rule.h>

class rule_list {
public:
    typedef unsigned int size_type;
    typedef std::vector<const rule*> vector_type;
    typedef std::vector<hypothesis*> hypothesis_list;

    rule_list(size_type size = 0);
    ~rule_list();

    void set_rule_vector(const vector_type* vec);
    void add_hypothese_list(hypothesis_list* list);
    std::vector<const rule*>* get_rule_vector();
    hypothesis_list* get_hypothesis_vector(unsigned int pos);
    size_type get_rule_nonterminal_num() const;
    size_type size() const;
    const symbol* get_start_symbol() const;
private:
    std::vector<const rule*>* rule_vector;
    std::vector<hypothesis_list*>* hypothesis_vector;
};

class rule_set {
public:
    typedef unsigned int size_type;

    rule_set();
    ~rule_set();

    void add_complete_rule(partial_rule* rule);
    void clear();
    size_type size() const;
    rule_list* get_rule_list(size_type index) const;
private:
    std::vector<rule_list*> applicable_rule_set;
};

#endif /* __RULE_SET_H__ */
