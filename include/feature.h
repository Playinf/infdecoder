/*
 * feature.h
 *
 *
 * author: Playinf
 * email: playinf@stu.xmu.edu.cn
 *
 */
#ifndef __FEATURE_H__
#define __FEATURE_H__

#include <feature_function.h>

class hypothesis;

class feature {
public:
    feature(unsigned int id);
    ~feature();

    float get_score() const;
    float get_weight() const;
    unsigned int get_id() const;

    void evaluate(const hypothesis* hypo);
private:
    float score;
    float weight;
    unsigned int feature_id;
    feature_function feature_func;
};

#endif /* __FEATURE_H__ */
