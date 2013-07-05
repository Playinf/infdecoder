/* unknow_word_rule.cpp */
#include <cmath>
#include <string>
#include <vector>
#include <rule.h>
#include <symbol.h>

rule* create_unknow_word_rule(const symbol* word)
{
    rule* r = new rule(1);
    symbol_table* table = symbol_table::get_instance();
    const symbol* x_symbol = table->search_symbol("X", false);
    std::vector<float> scores;

    scores.push_back(-10.0);
    scores.push_back(-10.0);
    scores.push_back(-10.0);
    scores.push_back(-10.0);
    scores.push_back(std::log(2.718));

    r->add_symbol(word);
    r->set_start_symbol(x_symbol, x_symbol);
    r->set_score(scores);

    return r;
}
