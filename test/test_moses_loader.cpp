/* test_moses_loader.cpp */
#include <string>
#include <vector>
#include <iostream>
#include <rule.h>
#include <lexer.h>
#include <model.h>
#include <config.h>
#include <parser.h>
#include <trellis.h>
#include <verbose.h>
#include <utility.h>
#include <parameter.h>

void load_moses_model();
void moses_ini_loader(const char* filename);
rule* create_unknow_word_rule(const symbol* sym);
void load_moses_options(int argc, char** argv);

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

int main(int argc, char** argv)
{
    std::string sentence = "北韩 是 与 澳洲 有 邦交 的 少数 国家 之一";
    configuration* config = configuration::get_instance();
    model* system_model = config->get_model();
    std::vector<std::string> vec;
    std::vector<rule_tree*> tree_vec;
    std::vector<hypothesis*> hypo_vec;
    std::vector<std::shared_ptr<const trellis_path>> path_list;
    std::vector<const std::string*> output_vec;
    lexer lex;

    load_moses_options(argc, argv);
    config->load_parameter();
    config->load_weight();
    print_parameter();
    load_moses_model();

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
    translator.get_nbest(1000, &path_list, true);

    for (unsigned int i = 0; i < path_list.size(); i++) {
        auto& path = path_list[i];
        output_vec.clear();
        path->output(&output_vec);

        std::cout << i << " ||| ";

        for (unsigned int i = 0; i < output_vec.size(); i++) {
            std::cout << *output_vec[i] << " ";
        }

        auto score_vec = path->get_score_vector();
        std::cout << " ||| ";
        for (unsigned int i = 0; i < score_vec->size(); i++) {
            std::cout << score_vec->at(i) << " ";
        }

        std::cout << " ||| " << path->get_total_score();
        std::cout << " ||| " << path->get_heuristic_score();
        std::cout << std::endl;
    }
}
