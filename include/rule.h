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

    unsigned int get_id() const;
    float get_heuristic_score() const;
    float get_score(unsigned int index) const;
    size_type get_terminal_number() const;
    const symbol* get_start_symbol(int index) const;
    const std::vector<const symbol*>& get_target_rule_body() const;
    unsigned int get_nonterminal_number() const;
    unsigned int get_nonterminal_map(unsigned int src) const;

    void add_symbol(const symbol* sym);
    void set_heuristic_score(float score);
    void set_score(std::vector<float>& vec);
    void add_align(unsigned int src, unsigned int tgt);
    void set_rule_tree_id(unsigned int id);
    void set_start_symbol(const symbol* src, const symbol* tgt);
private:
    float* score;
    float heuristic_score;
    const symbol* start_symbol[2];
    std::vector<const symbol*> target_rule_body;
    unsigned int id;
    unsigned int* nonterminal_map;
    unsigned int target_terminal_number;
};

#endif /* __RULE_H__ */
