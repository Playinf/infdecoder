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

#include <string>

class hypothesis;

class feature {
public:
    typedef double (*feature_function)(const hypothesis* h, unsigned int id);

    feature(unsigned int id, feature_function func, double score = 0.0)
    {
        this->id = id;
        this->score = score;
        function = func;
    }

    ~feature()
    {
    }

    void evaluate(const hypothesis* hypo)
    {
        score = function(hypo, id);
    }

    double get_score() const
    {
        return score;
    }

    unsigned int get_id() const
    {
        return id;
    }

private:
    feature_function function;
    double score;
    unsigned int id;
};

#endif /* __FEATURE_H__ */
