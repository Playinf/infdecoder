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

#include <string>

class beam;
class rule;
class symbol;
class cube_item;
class chart_cell;
class hypothesis;
class partial_rule;
class translation_option;
class translation_option_set;

void to_string(const beam* b, std::string& out);
void to_string(const rule* r, std::string& out);
void to_string(const symbol* s, std::string& out);
void to_string(const cube_item* i, std::string& out);
void to_string(const chart_cell* c, std::string& out);
void to_string(const hypothesis* h, std::string& out);
void to_string(const partial_rule* r, std::string& out);
void to_string(const translation_option* o, std::string& out);
void to_string(const translation_option_set* s, std::string& out);

void print_beam(const beam* b);
void print_rule(const rule* r);
void print_symbol(const symbol* s);
void print_cube_item(const cube_item* i);
void print_chart_cell(const chart_cell* c);
void print_hypothesis(const hypothesis* h);
void print_partial_rule(const partial_rule* r);
void print_translation_option(const translation_option* option);
void print_translation_option_set(const translation_option_set* set);

void hypothesis_track(const hypothesis* h);
void hypothesis_track(const hypothesis* h, std::string& out);

#endif /* __VERBOSE_H__ */
