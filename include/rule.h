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
#include <feature.h>

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

    const symbol* get_start_symbol(int index) const;
    size_type get_terminal_number() const;
    const std::vector<const symbol*>& get_target_rule_body() const;
    const feature* get_feature(unsigned int id) const;
    void set_start_symbol(const symbol* src, const symbol* tgt);
    void add_symbol(const symbol* sym);
    void add_feature(feature* fea);
    float get_score() const;
    void evaluate_score(const language_model* lm, const weight_vector& weight);
private:
    float score;
    const symbol* start_symbol[2];
    std::vector<feature> feature_set;
    std::vector<const symbol*> target_rule_body;
    unsigned int* nonterminal_map;
    unsigned int target_terminal_number;
};

#endif /* __RULE_H__ */
