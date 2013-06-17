/*
 * parser.h
 * implementation of CYK parser
 *
 * author: Playinf
 * playinf@stu.xmu.edu.cn
 *
 */
#ifndef __PARSER_H__
#define __PARSER_H__

#include <memory>
#include <vector>
#include <string>
#include <rule.h>
#include <chart.h>
#include <trellis.h>
#include <rule_set.h>
#include <rule_tree.h>
#include <rule_finder.h>
#include <partial_rule.h>

class derivation;

class parser {
public:
    typedef unsigned int size_type;
    typedef std::vector<const symbol*> input_type;
    typedef std::vector<std::shared_ptr<const trellis_path>> path_vector;

    parser(std::vector<rule_tree*>* tree_vec);
    ~parser();

    void parse(input_type& input);
    hypothesis* get_hypothesis(size_type index);
    void get_nbest(unsigned int num, path_vector* path_list, bool distinct);
    void find_nbest(unsigned int num, std::vector<derivation*>* vec, bool distinct);
    void clear();
private:
    void process_unknow_word(const symbol* sym);
    void parser_initialize();
    chart* table;
    input_type* input;
    rule_set* applicable_rule_set;
    std::vector<rule_tree*>* tree_vector;
    std::vector<rule_finder*> rule_lookup_manager;
};

#endif /* __PARSER_H__ */
