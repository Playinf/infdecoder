/*
 * symbol.h
 *
 *
 * author: Playinf
 * playinf@stu.xmu.edu.cn
 *
 */
#ifndef __SYMBOL_H__
#define __SYMBOL_H__

#include <string>
#include <unordered_set>

enum symbol_type {
    TERMINAL, NONTERMINAL
};

class symbol {
public:
    typedef unsigned int size_type;

    symbol();
    symbol(const std::string& name, bool terminal = true);
    symbol(const std::string& name, symbol_type type);
    ~symbol();

    const std::string* get_name() const;
    symbol_type get_type() const;
    bool is_terminal() const;
    bool is_nonterminal() const;
    bool operator==(const symbol& sym) const;
private:
    void operator=(const symbol& sym);
    const std::string* name;
    symbol_type type;
};

struct symbol_hash {
    size_t operator()(const symbol& sym) const;
    size_t operator()(const symbol* sym) const;
};

class symbol_table {
public:
    static const symbol* search_symbol(std::string& s);
    static const symbol* search_symbol(std::string& s, bool terminal);
private:
    static std::unordered_set<symbol, symbol_hash> symbol_set;
    static std::unordered_set<std::string> string_set;
};

#endif
