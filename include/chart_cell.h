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

#include <vector>
#include <utility>

class beam;
class symbol;
class rule_set;
class hypothesis;

class chart_cell {
public:
    typedef std::vector<hypothesis*> hypothesis_list;
    chart_cell();
    ~chart_cell();

    void add_hypothesis(hypothesis* hypo);
    void decode(rule_set* s);
    hypothesis_list* get_hypothesis_list(const symbol* lhs);
    const std::vector<const symbol*>& get_symbol_set() const;
    void sort();
    void print();
private:
    std::vector<const symbol*> symbol_set;
    std::map<const symbol*, beam*> nonterminal_beam_set;
};

#endif /* __CHART_CELL_H__ */
