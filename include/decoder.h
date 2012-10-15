/*
 * decoder.h
 *
 *
 * author: Playinf
 * playinf@stu.xmu.edu.cn
 *
 */
#ifndef __DECODER_H__
#define __DECODER_H__

#include <vector>
#include <parser.h>
#include <symbol.h>
#include <rule_tree.h>

class decoder {
public:
    typedef void (*io_function)(void* args);

    decoder();
    ~decoder();

    void translate(io_function func);
    void load_rule_table(io_function func);
    void load_glue_rules();
private:
    unsigned int nbest;
    parser* syntax_analyzer;
    std::vector<rule_tree*> tree_vector;
};

#endif /* __DECODER_H__ */
