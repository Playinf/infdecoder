/*
 * rule_finder.h
 *
 *
 * author: Playinf
 * playinf@stu.xmu.edu.cn
 *
 */
#ifndef __RULE_FINDER_H__
#define __RULE_FINDER_H__

#include <chart.h>
#include <rule_set.h>
#include <rule_tree.h>
#include <partial_rule.h>

class rule_finder {
public:
    typedef unsigned int size_type;
    typedef std::vector<const symbol*> input_type;

    rule_finder(rule_tree* tree);
    ~rule_finder();

    void set_span_limit(size_type limit);
    size_type get_span_limit() const;
    void initialize(input_type* input, chart* table);
    void find(size_type start, size_type end, rule_set& rules);
    void clear();
private:
    chart* table;
    rule_tree* tree;
    input_type* input;
    size_type span_limit;
    std::vector<partial_rule_set*>* partial_rule_table;
};

#endif
