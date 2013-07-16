/* test_rule_tree_prune.cpp */
#include <vector>
#include <iostream>
#include <lexer.h>
#include <model.h>
#include <config.h>
#include <parser.h>
#include <trellis.h>
#include <verbose.h>
#include <utility.h>
#include <parameter.h>
#include <rule_tree.h>
#include <hypothesis.h>
#include <language_model.h>
#include <feature_function.h>

void moses_format_loader(void* args);
rule* create_unknow_word_rule(const symbol* sym);
float hpb_rule_heuristic_function(const rule* r);

void load_rule_from_file(const char* filename, rule_tree* table)
{
    void** arg_arr = new void*[2];

    arg_arr[0] = (void*) filename;
    arg_arr[1] = (void*) table;

    moses_format_loader((void*) arg_arr);

    std::cout << "rule number: " << table->get_rule_number() << std::endl;
    std::cout << "node number: " << table->get_node_number() << std::endl;

    delete[] arg_arr;
}

void load_parameter()
{
    configuration* config = configuration::get_instance();
    parameter* param = config->get_parameter();

    param->add_parameter("rule_table", "rule-table");
    param->add_parameter("special_rule_table", "glue-grammar");
    param->add_parameter("language_model", "30K.gz");
    param->add_parameter("language_model_order", 5u);
    param->add_parameter("beam_size", 1000u);
    config->load_parameter();
}

void load_model()
{
    configuration* config = configuration::get_instance();
    model* system_model = config->get_model();
    parameter* param = config->get_parameter();

    auto& lm_filename = param->get_string_parameter("language_model");
    auto& table_filename = param->get_string_parameter("rule_table");
    auto& glue_filename = param->get_string_parameter("special_rule_table");

    language_model* lm = new language_model();
    rule_tree* table = new rule_tree();
    rule_tree* glue_table = new rule_tree();

    table->set_id(0);
    glue_table->set_id(1);

    lm->set_order(param->get_integer_parameter("language_model_order"));
    lm->load(lm_filename.c_str());
    lm->set_id(0);

    load_rule_from_file(table_filename.c_str(), table);
    load_rule_from_file(glue_filename.c_str(), glue_table);

    unsigned int id;

    /* language model */
    id = system_model->add_feature("language model", "main language model");
    system_model->add_language_model(lm);
    system_model->set_feature_function(id, language_model_feature_function);
    system_model->set_feature_source(id, lm);
    lm->set_feature(id);

    /* rule table */
    system_model->add_rule_table(table);
    id = system_model->add_feature("inverse translation probability", "rule");
    system_model->set_feature_function(id, translation_model_feature_function);
    system_model->set_feature_source(id, table);
    table->add_feature(id);
    id = system_model->add_feature("inverse lexical weight", "rule");
    system_model->set_feature_function(id, translation_model_feature_function);
    system_model->set_feature_source(id, table);
    table->add_feature(id);
    id = system_model->add_feature("translation probability", "rule");
    system_model->set_feature_function(id, translation_model_feature_function);
    system_model->set_feature_source(id, table);
    table->add_feature(id);
    id = system_model->add_feature("lexical weight", "rule");
    system_model->set_feature_function(id, translation_model_feature_function);
    system_model->set_feature_source(id, table);
    table->add_feature(id);
    id = system_model->add_feature("rule penalty", "rule");
    system_model->set_feature_function(id, translation_model_feature_function);
    system_model->set_feature_source(id, table);
    table->add_feature(id);

    /* glue grammar */
    system_model->add_rule_table(glue_table);
    id = system_model->add_feature("glue rule penalty", "glue grammar");
    system_model->set_feature_function(id, translation_model_feature_function);
    system_model->set_feature_source(id, glue_table);
    glue_table->add_feature(id);

    /* word penalty */
    id = system_model->add_feature("word penalty", "build in word penalty");
    system_model->set_feature_function(id, word_penalty_feature_function);

    /* weight */
    system_model->push_weight(1.0);
    system_model->push_weight(1.0);
    system_model->push_weight(1.0);
    system_model->push_weight(1.0);
    system_model->push_weight(1.0);
    system_model->push_weight(1.0);
    system_model->push_weight(1.0);
    system_model->push_weight(1.0);

    /* rule tree pruning */
    table->set_heuristic_function(hpb_rule_heuristic_function);
    table->sort(20);
    table->prune(20);
    std::cout << "rule number: " << table->get_rule_number() << std::endl;
}

void print_parameter()
{
    configuration* config = configuration::get_instance();
    parameter* param = config->get_parameter();
    std::vector<std::pair<std::string, std::string>> vec;

    param->get_parameter(vec);

    for (auto& val : vec) {
        std::cout << val.first << ":" << val.second << std::endl;
    }
}

int main()
{
    std::string sentence = "北韩 是 与 澳洲 有 邦交 的 少数 国家 之一";
    std::vector<std::string> vec;
    std::vector<rule_tree*> tree_vec;
    std::vector<hypothesis*> hypo_vec;
    configuration* config = configuration::get_instance();
    model* system_model = config->get_model();
    lexer lex;
    std::vector<std::shared_ptr<const trellis_path>> path_list;
    std::vector<const std::string*> output_vec;

    load_parameter();
    print_parameter();
    load_model();

    string_split(sentence, " ", vec);
    lex.set_handler(create_unknow_word_rule);
    lex.set_input(vec);
    lex.process();
    tree_vec.push_back(lex.get_rule_tree());
    tree_vec.push_back(system_model->get_rule_tree(0));
    tree_vec.push_back(system_model->get_rule_tree(1));

    parser translator(&tree_vec);
    translator.parse(lex.get_output());
    translator.get_all_hypothesis(hypo_vec);
}
