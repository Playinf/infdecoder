/*
 * penalty_model.h
 *
 *
 * author: Playinf
 * email: playinf@stu.xmu.edu.cn
 *
 */
#ifndef __PENALTY_H__
#define __PENALTY_H__

class word_penalty_model {
public:
    word_penalty_model();
    ~word_penalty_model();

    word_penalty_model(const word_penalty_model& m) = delete;
    word_penalty_model& operator=(const word_penalty_model& m) = delete;

    float get_penalty() const;
    unsigned int get_id() const;

    void set_id(unsigned int id);
    void set_penalty(float score);
private:
    float penalty;
    unsigned int id;
};

#endif /* __PENALTY_H__ */
