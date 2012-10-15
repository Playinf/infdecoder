/* parser.cpp */
#include <parser.h>
#include <config.h>
#include <feature.h>
#include <string>
#include <cstdlib>

#include <iostream>

parser::parser(std::vector<rule_tree*>* tree_vec)
{
    input = nullptr;
    table = nullptr;
    applicable_rule_set = nullptr;
    tree_vector = tree_vec;

    unsigned int size = tree_vec->size();

    for (unsigned int i = 0; i < size; i++) {
        rule_tree* tree = tree_vector->at(i);
        rule_finder* finder = new rule_finder(tree);
        rule_lookup_manager.push_back(finder);
    }

    std::string span_limit = parameter::get_parameter("span-limit");
    unsigned int limit = std::atoi(span_limit.c_str());
    rule_lookup_manager[0]->set_span_limit(limit);
    rule_lookup_manager[1]->set_span_limit(0);
}

parser::~parser()
{
    clear();

    unsigned int size = rule_lookup_manager.size();

    for (unsigned int i = 0; i < size; i++)
        delete rule_lookup_manager[i];
}

void parser::parser_initialize()
{
    unsigned int size = input->size();
    unsigned int n = rule_lookup_manager.size();

    clear();
    table = new chart(size);
    applicable_rule_set = new rule_set();

    for (unsigned int i = 0; i < n; i++) {
        rule_finder* finder = rule_lookup_manager[i];
        finder->initialize(input, table);
    }
}

void parser::clear()
{
    unsigned int size = rule_lookup_manager.size();

    for (unsigned int i = 0; i < size; i++) {
        rule_finder* finder = rule_lookup_manager[i];
        finder->clear();
    }

    if (table) {
        delete table;
        table = nullptr;
    }

    if (applicable_rule_set) {
        delete applicable_rule_set;
        applicable_rule_set = nullptr;
    }
}

void parser::parse(input_type& input)
{
    size_type n = input.size();
    size_type size = rule_lookup_manager.size();
    chart_cell* cell;

    this->input = &input;
    parser_initialize();

    for (size_type len = 1; len <= n; len++) {

        for (size_type i = 0; i <= n - len; i++) {
            size_type j = i + len - 1;

            //std::cout << "[" << i << ", " << j << "]\n";
            
            if (len == 1 && i != 0 && i != len - 1) {
                const symbol* sym = input[i];
                process_unknow_word(sym);
            }

            cell = table->get_cell(i, j);

            for (unsigned int k = 0; k < size; k++) {
                rule_finder* finder = rule_lookup_manager[k];
                unsigned int span_limit = finder->get_span_limit();

                if (span_limit && len > span_limit)
                    continue;

                applicable_rule_set->clear();
                finder->find(i, j, *applicable_rule_set);
                cell->decode(applicable_rule_set);
                cell->sort();
            }

            //cell->print();
        }
    }
}

hypothesis* parser::get_hypothesis(size_type index)
{
    static std::string s_symbol_name("S");
    const symbol* s_symbol;
    std::vector<const std::string*> str_vec;
    
    chart_cell* cell = table->get_cell(0, input->size() - 1);
    s_symbol = symbol_table::search_symbol(s_symbol_name, false);
    
    auto hypothesis_list = cell->get_hypothesis_list(s_symbol);
    unsigned int size = hypothesis_list->size();

    if (index >= size)
        return nullptr;

    return hypothesis_list->at(index);
}

void parser::process_unknow_word(const symbol* sym)
{
    static rule_tree* tree = tree_vector->at(0);
    static const rt_node* root = tree->get_root();
    static std::string x_symbol_name("X");
    static std::string trans_des = "translation probablity";
    static std::string rev_trans_des = "reverse translation probablity";
    static std::string lex_des = "lexical weight";
    static std::string rev_lex_des = "reverse lexical weight";
    unsigned int id[4];
    const symbol* x_symbol;

    id[0] = model::get_feature_id(trans_des);
    id[1] = model::get_feature_id(rev_trans_des);
    id[2] = model::get_feature_id(lex_des);
    id[3] = model::get_feature_id(rev_lex_des);
    
    x_symbol = symbol_table::search_symbol(x_symbol_name, false);
    rt_node* child = tree->find_child(root, sym);

    if (child == nullptr || child->get_rules() == nullptr) {
        unsigned int fea_num = 4;
        rule* oov_rule = new rule(1);
        oov_rule->add_symbol(sym);

        for (unsigned int i = 0; i < fea_num; i++) {
            feature fea(id[i], translation_feature_function, -10.0);
            oov_rule->add_feature(&fea);
        }

        oov_rule->set_start_symbol(x_symbol);
        oov_rule->set_order(false);
        oov_rule->evaluate_score();

        child = tree->insert_node(const_cast<rt_node*>(root), sym);
        tree->insert_rule(child, oov_rule);
    }
}
