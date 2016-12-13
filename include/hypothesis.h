/*
 * hypothesis.h
 *
 *
 * author: Playinf
 * email: playinf@stu.xmu.edu.cn
 *
 */
#ifndef __HYPOTHESIS_H__
#define __HYPOTHESIS_H__

#include <string>
#include <vector>
#include <utility>
#include <feature.h>

class rule;
class symbol;
class feature;
class chart_cell;
class information;
class partial_rule;

class hypothesis {
public:
    typedef unsigned int size_type;

    hypothesis(const rule* r);
    ~hypothesis();

    hypothesis(const hypothesis*) = delete;
    hypothesis& operator=(const hypothesis*) = delete;

    const rule* get_rule() const;
    float get_total_score() const;
    information* get_input() const;
    unsigned int get_terminal_number() const;
    unsigned int get_feature_number() const;
    const partial_rule* get_partial_rule() const;
    const feature* get_feature(unsigned int index) const;
    const symbol* get_start_symbol(unsigned int index) const;
    hypothesis* get_recombined_hypothesis() const;
    size_type get_previous_hypothesis_number() const;
    std::vector<hypothesis*>* get_hypothesis_vector() const;
    hypothesis* get_previous_hypothesis(unsigned int index) const;
    const std::pair<unsigned int, unsigned int>& get_span() const;

    void evaluate_score();
    void recombine(hypothesis* hypo);
    int compare(const hypothesis* hypo) const;

    void set_input(information* input);
    void set_partial_rule(const partial_rule *rule);
    void set_span(unsigned int start, unsigned int end);
    void set_previous_hypothesis(unsigned int index, hypothesis* h);
private:
    float score;
    information* input;
    const rule* target_rule;
    unsigned int terminal_number;
    const partial_rule* source_rule;
    hypothesis* recombined_hypothesis;
    std::vector<feature> log_linear_model;
    std::pair<unsigned int, unsigned int> span;
    std::vector<hypothesis*>* hypothesis_vector;

};

#endif /* __HYPOTHESIS_H__ */
