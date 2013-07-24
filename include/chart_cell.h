/*
 * chart_cell.h
 *
 *
 * author: Playinf
 * email: playinf@stu.xmu.edu.cn
 *
 */
#ifndef __CHART_CELL_H__
#define __CHART_CELL_H__

#include <map>
#include <set>
#include <vector>
#include <utility>

class beam;
class symbol;
class hypothesis;
class translation_option_set;

class chart_cell {
public:
    typedef std::set<const symbol*> symbol_set;
    typedef std::vector<hypothesis*> hypothesis_list;
    chart_cell();
    ~chart_cell();

    const symbol_set& get_source_start_symbol_set() const;
    const symbol_set& get_target_start_symbol_set() const;
    void get_all_hypothesis(hypothesis_list* hypo_list) const;
    hypothesis_list* get_hypothesis_list(const symbol* start_symbol);

    void sort();
    void add_hypothesis(hypothesis* hypo);
    void decode(translation_option_set* s, unsigned int limit);
private:
    symbol_set source_start_symbol_set;
    symbol_set target_start_symbol_set;
    std::map<const symbol*, beam*> nonterminal_beam_set;
};

#endif /* __CHART_CELL_H__ */
