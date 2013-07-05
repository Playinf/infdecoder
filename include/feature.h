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

class hypothesis;

class feature {
public:
    feature(unsigned int id);
    ~feature();

    float get_score() const;
    unsigned int get_id() const;

    void evaluate(const hypothesis* hypo);
private:
    float score;
    unsigned int feature_id;
};

#endif /* __FEATURE_H__ */
