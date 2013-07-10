/*
 * rule.h
 *
 *
 * author: Playinf
 * email: playinf@stu.xmu.edu.cn
 *
 */
#ifndef __RULE_H__
#define __RULE_H__

#include <map>
#include <vector>

class symbol;
class language_model;

class rule {
public:
    typedef unsigned int size_type;
    typedef std::vector<float> weight_vector;

    rule(size_type size);
    ~rule();

    rule(const rule& r) = delete;
    rule& operator=(const rule& r) = delete;

    float get_total_score() const;
    float get_score(unsigned int index) const;
    unsigned int get_rule_tree_id() const;
    size_type get_terminal_number() const;
    const symbol* get_start_symbol(int index) const;
    const std::vector<const symbol*>& get_target_rule_body() const;
    unsigned int get_nonterminal_map(unsigned int src) const;

    void add_symbol(const symbol* sym);
    void set_score(std::vector<float>& vec);
    void add_align(unsigned int src, unsigned int tgt);
    void set_start_symbol(const symbol* src, const symbol* tgt);
    void set_rule_tree_id(unsigned int id);
private:
    float* score;
    unsigned int id;
    const symbol* start_symbol[2];
    std::vector<const symbol*> target_rule_body;
    unsigned int* nonterminal_map;
    unsigned int target_terminal_number;
};

#endif /* __RULE_H__ */
