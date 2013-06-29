/*
 * verbose.h
 * verbose output for debugging purpose
 *
 * author: Playinf
 * email: playinf@stu.xmu.edu.cn
 *
 */
#ifndef __VERBOSE_H__
#define __VERBOSE_H__

#define __VERBOSE__

class beam;
class rule;
class symbol;
class hypothesis;

void print_beam(const beam* b);
void print_rule(const rule* r);
void print_symbol(const symbol* s);
void print_hypothesis(const hypothesis* h);
void hypothesis_track(const hypothesis* h);

#endif /* __VERBOSE_H__ */
