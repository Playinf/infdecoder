/* moses_oov_rule.cpp */
#include <cmath>
#include <string>
#include <vector>
#include <rule.h>
#include <symbol.h>
#include <alignment.h>

rule* create_unknown_word_rule(const symbol* word)
{
    rule* r = new rule(1);
    symbol_table* table = symbol_table::get_instance();
    const symbol* x_symbol = table->search_symbol("X", false);
    std::vector<float> scores;
    alignment align(1, 0);

    align.add_alignment(0, 0);
    alignment_table* align_table = alignment_table::get_instance();
    const alignment* align_ptr = align_table->search_alignment(align);

    r->add_symbol(word);
    r->set_alignment(align_ptr);
    r->set_start_symbol(x_symbol, x_symbol);
    r->set_score(scores);

    return r;
}

rule* create_null_word_rule(const symbol* word)
{
    rule* r = new rule(1);
    symbol_table* table = symbol_table::get_instance();
    const symbol* x_symbol = table->search_symbol("X", false);
    std::vector<float> scores;
    alignment align(0, 0);

    alignment_table* align_table = alignment_table::get_instance();
    const alignment* align_ptr = align_table->search_alignment(align);

    r->set_alignment(align_ptr);
    r->set_start_symbol(x_symbol, x_symbol);
    r->set_score(scores);

    return r;
}
