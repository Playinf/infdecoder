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

    float get_score() const;
    float get_heuristic_score() const;
    float get_total_score() const;
    const rule* get_rule() const;
    unsigned int get_terminal_number() const;
    unsigned int get_feature_number() const;
    const feature* get_feature(unsigned int index) const;
    const symbol* get_start_symbol(unsigned int index) const;
    hypothesis* get_recombined_hypothesis() const;
    size_type get_previous_hypothesis_number() const;
    std::vector<hypothesis*>* get_hypothesis_vector() const;
    hypothesis* get_previous_hypothesis(unsigned int index) const;
    const std::vector<const std::string*>* get_prefix() const;
    const std::vector<const std::string*>* get_suffix() const;

    void evaluate_score();
    void set_heuristic_score(float score);
    void recombine(hypothesis* hypo);
    void push_hypothesis(hypothesis* h);
    void output(std::vector<const std::string*>& s) const;
    int compare(const hypothesis* hypo) const;
    void calculate_prefix_suffix(unsigned int order);
private:
    void calculate_prefix(std::vector<const std::string*>* out,
        size_type& size);
    void calculate_suffix(std::vector<const std::string*>* out,
        size_type& size);

    const rule* target_rule;
    hypothesis* recombined_hypothesis;
    std::pair<size_type, size_type> span;
    std::vector<const std::string*> prefix;
    std::vector<const std::string*> suffix;
    std::vector<hypothesis*>* hypothesis_vector;
    std::vector<feature> log_linear_model;
    unsigned int terminal_number;
    float score;
    float heuristic_score;
};

#endif /* __HYPOTHESIS_H__ */
