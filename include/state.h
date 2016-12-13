/*
 * state.h
 *
 *
 * author: Playinf
 * email: playinf@stu.xmu.edu.cn
 *
 */
#ifndef __STATE_H__
#define __STATE_H__

class state {
public:
    state() {} ;
    virtual ~state() {};

    state(const state&) = delete;
    state& operator=(const state&) = delete;

    virtual int compare(const state* s) const = 0;
};

#endif /* __STATE_H__ */
