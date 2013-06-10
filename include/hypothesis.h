/*
 * hypothesis.h
 *
 *
 * author: Playinf
 * playinf@stu.xmu.edu.cn
 *
 */
#ifndef __HYPOTHESIS_H__
#define __HYPOTHESIS_H__

#include <string>
#include <vector>
#include <utility>
#include <model.h>

class rule;
class symbol;
class feature;
class model;

class hypothesis {
public:
    typedef unsigned int size_type;

    hypothesis(rule* r);
    ~hypothesis();

    const symbol* get_start_symbol() const;
    void push_hypothesis(hypothesis* h);
    void recombine(hypothesis* hypo);
    void set_next_hypothesis(hypothesis* hypo);
    const hypothesis* get_next_hypothesis() const;
    void evaluate_score();
    double get_score() const;
    double get_heuristic_score() const;
    double get_total_score() const;
    void set_heuristic_score(double score);
    void output(std::vector<const std::string*>& s);
    int compare(const hypothesis* hypo) const;
    const feature* get_feature(unsigned int id);
    const std::vector<const std::string*>* get_prefix() const;
    const std::vector<const std::string*>* get_suffix() const;
    size_type previous_hypothesis_number() const;
    hypothesis* get_previous_hypothesis(unsigned int index) const;
    hypothesis* get_recombined_hypothesis() const;
    unsigned int get_terminal_number() const;
    const rule* get_rule() const;
private:
    void calculate_prefix(std::vector<const std::string*>* out,
        size_type& size);
    void calculate_suffix(std::vector<const std::string*>* out,
        size_type& size);

    rule* target_rule;
    hypothesis* next_hypothesis;
    hypothesis* recombined_hypothesis;
    std::pair<size_type, size_type> span;
    std::vector<const std::string*> prefix;
    std::vector<const std::string*> suffix;
    std::vector<hypothesis*>* hypothesis_vector;
    unsigned int terminal_number;
    model log_linear_model;
    double score;
    double heuristic_score;
};

#endif /* __HYPOTHESIS_H__ */
