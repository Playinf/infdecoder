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

class hypothesis {
public:
    typedef unsigned int size_type;

    hypothesis(const rule* r);
    ~hypothesis();

    hypothesis(const hypothesis*) = delete;
    hypothesis& operator=(const hypothesis*) = delete;

    const rule* get_rule() const;
    float get_total_score() const;
    unsigned int get_terminal_number() const;
    unsigned int get_feature_number() const;
    const feature* get_feature(unsigned int index) const;
    const symbol* get_start_symbol(unsigned int index) const;
    hypothesis* get_recombined_hypothesis() const;
    size_type get_previous_hypothesis_number() const;
    std::vector<hypothesis*>* get_hypothesis_vector() const;
    hypothesis* get_previous_hypothesis(unsigned int index) const;

    void evaluate_score();
    void recombine(hypothesis* hypo);
    int compare(const hypothesis* hypo) const;
    void set_previous_hypothesis(unsigned int index, hypothesis* h);
private:
    float score;
    const rule* target_rule;
    unsigned int terminal_number;
    hypothesis* recombined_hypothesis;
    std::vector<feature> log_linear_model;
    std::vector<hypothesis*>* hypothesis_vector;
};

#endif /* __HYPOTHESIS_H__ */
