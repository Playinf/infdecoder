/*
 * lexer.h
 *
 * author: Playinf
 * email: playinf@stu.xmu.edu.cn
 *
 */
#ifndef __LEXER_H__
#define __LEXER_H__

#include <string>
#include <vector>

class rule;
class symbol;
class rule_tree;

typedef rule* (*unknow_word_handler)(const symbol* word);

class lexer {
public:
    lexer();
    ~lexer();

    lexer(const lexer& lex) = delete;
    lexer& operator=(const lexer& lex) = delete;

    rule_tree* get_rule_tree() const;
    std::vector<const symbol*>& get_output();

    void process();
    void set_handler(unknow_word_handler handler);
    void set_input(std::vector<std::string>& input);
private:
    void process_unknow_word();
    rule_tree* unknow_word_tree;
    std::vector<std::string>* input;
    std::vector<const symbol*> sentence;
    unknow_word_handler handler;
};

#endif /* __LEXER_H__ */
