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
#include <hypothesis.h>

struct hypothesis_less {
    bool operator()(const hypothesis& h1, const hypothesis& h2)
    {
        double s1 = h1.get_total_score();
        double s2 = h2.get_total_score();

        return s1 > s2;
    }

    bool operator()(const hypothesis* h1, const hypothesis* h2)
    {
        double s1 = h1->get_total_score();
        double s2 = h2->get_total_score();

        return s1 > s2;
    }
};

struct hypothesis_compare {
    bool operator()(const hypothesis* h1, const hypothesis* h2)
    {
        int ret = h1->compare(h2);

        if (ret != 0)
            return ret < 0;

        return false;
    }
};

class beam {
public:
    typedef unsigned int size_type;
    typedef std::set<hypothesis*, hypothesis_compare> set_type;

    beam(size_type histogram, float threshold);
    ~beam();

    void set_parameter(size_type histogram, float threshold);
    void insert_hypothesis(hypothesis* hypo);
    std::vector<hypothesis*>* get_sorted_hypothesis_list();
    void sort();
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
