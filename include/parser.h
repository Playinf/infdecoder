/*
 * parser.h
 * implementation of CYK parser
 *
 * author: Playinf
 * email: playinf@stu.xmu.edu.cn
 *
 */
#ifndef __PARSER_H__
#define __PARSER_H__

#include <memory>
#include <vector>
#include <string>

class chart;
class symbol;
class rule_tree;
class hypothesis;
class information;
class rule_finder;
class trellis_path;
class translation_option_set;

class parser {
public:
    typedef unsigned int size_type;
    typedef std::vector<std::shared_ptr<const trellis_path>> path_vector;

    parser(std::vector<rule_tree*>* tree_vec);
    ~parser();

    parser(const parser&) = delete;
    parser& operator=(const parser&) = delete;

    hypothesis* get_best_hypothesis() const;
    void get_all_hypothesis(std::vector<hypothesis*>& hypo_vec) const;
    void get_nbest(unsigned int num, path_vector* path_list) const;

    void clear();
    void parse(information* input);
private:
    void parser_initialize();

    chart* table;
    information* input;
    std::vector<rule_tree*>* tree_vector;
    std::vector<rule_finder*> rule_lookup_manager;
    translation_option_set* applicable_translation_option_set;
};

#endif /* __PARSER_H__ */
