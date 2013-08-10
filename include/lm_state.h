/*
 * lm_state.h
 *
 *
 * author: Playinf
 * email: playinf@stu.xmu.edu.cn
 *
 */
#ifndef __LM_STATE_H__
#define __LM_STATE_H__

#include <state.h>

class lm_state : public state {
public:
    typedef unsigned int size_type;
    lm_state(const hypothesis* hypo, unsigned int id);
    ~lm_state();

    lm_state& operator=(const lm_state& s) = delete;

    float get_score() const;
    const std::vector<const std::string*>* get_prefix() const;
    const std::vector<const std::string*>* get_suffix() const;

    void set_score(float score);
    virtual int compare(const state* lmstate) const;
    virtual int compare(const lm_state* state) const;
    void calculate_prefix_suffix(unsigned int order);
private:
    float score;
    void* context;
    unsigned int id;
    const hypothesis* state_hypothesis;
    std::vector<const std::string*> prefix;
    std::vector<const std::string*> suffix;
};

#endif /* __LM_STATE_H__ */
