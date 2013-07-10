/*
 * beam.h
 * implementation of beam search
 *
 * author: Playinf
 * email: playinf@stu.xmu.edu.cn
 *
 */
#ifndef __BEAM_H__
#define __BEAM_H__

#include <set>
#include <queue>
#include <vector>
#include <functional.h>

class hypothesis;

class beam {
public:
    typedef unsigned int size_type;
    typedef std::set<hypothesis*, hypothesis_compare> set_type;

    beam(size_type histogram, float threshold);
    ~beam();

    beam(const beam& b) = delete;
    beam& operator=(const beam& b) = delete;

    std::vector<hypothesis*>* get_sorted_hypothesis_list();

    void sort();
    void set_parameter(size_type histogram, float threshold);
    void insert_hypothesis(hypothesis* hypo);
private:
    void prune();

    bool sorted;
    float max_score;
    float threshold;
    size_type beam_size;
    set_type hypothesis_set;
    std::vector<hypothesis*> ordered_hypothesis_list;
};

#endif /* __BEAM_H__ */
