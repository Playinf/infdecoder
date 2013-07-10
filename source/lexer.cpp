/* lexer.cpp */
#include <string>
#include <vector>
#include <lexer.h>
#include <symbol.h>
#include <rule_tree.h>

lexer::lexer()
{
    unknow_word_tree = nullptr;
}

lexer::~lexer()
{
    delete unknow_word_tree;
}

rule_tree* lexer::get_rule_tree() const
{
    return unknow_word_tree;
}

std::vector<const symbol*>& lexer::get_output()
{
    return sentence;
}

void lexer::process()
{
    symbol_table* table = symbol_table::get_instance();
    const symbol* bos = table->search_symbol("<s>");
    const symbol* eos = table->search_symbol("</s>");
    unsigned int size = input->size();
    rt_node* root;

    unknow_word_tree = new rule_tree();
    root = const_cast<rt_node*>(unknow_word_tree->get_root());

    sentence.push_back(bos);

    for (unsigned int i = 0; i < size; i++) {
        rt_node* p;
        const std::string& s = input->at(i);
        const symbol* sym = table->search_symbol(s);
        rule* r = handler(sym);

        sentence.push_back(sym);
        //p = unknow_word_tree->insert_node(root, sym);
        //unknow_word_tree->insert_rule(p, r);
    }

    sentence.push_back(eos);
}

void lexer::set_handler(unknow_word_handler handler)
{
    this->handler = handler;
}

void lexer::set_input(std::vector<std::string>& input)
{
    this->input = &input;
    sentence.reserve(input.size() + 2);
}
