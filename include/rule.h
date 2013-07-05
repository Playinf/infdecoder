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
    unsigned int get_feature_number() const;
    size_type get_terminal_number() const;
    const symbol* get_start_symbol(int index) const;
    const std::vector<const symbol*>& get_target_rule_body() const;

    void add_symbol(const symbol* sym);
    void set_score(std::vector<float>& vec);
    void set_start_symbol(const symbol* src, const symbol* tgt);
private:
    float* score;
    unsigned int feature_number;
    const symbol* start_symbol[2];
    std::vector<const symbol*> target_rule_body;
    unsigned int* nonterminal_map;
    unsigned int target_terminal_number;
};

#endif /* __RULE_H__ */
