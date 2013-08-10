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

    state(const state& s) = delete;
    state& operator=(const state& s) = delete;

    virtual int compare(const state* s) const = 0;
};

#endif /* __STATE_H__ */
