/* main.cpp */
#include <ctime>
#include <string>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <lm.h>
#include <model.h>
#include <config.h>
#include <format.h>
#include <decoder.h>
#include <utility.h>
#include <rule_set.h>

/* TODO:
 * version 0.1
 * 1. add nbest support
 * 2. add multi-thread support
 * 3. add dynamic linked feature function
 * 4. add dynamic linked io function
 */

void print_help()
{
    std::cout << "infdecoder version 0.1 beta" << std::endl;
    std::cout << "Usage:" << std::endl;
    std::cout << "\t-c | --config \t configuration file name" << std::endl;
    std::cout << "\t-h | --help \t show this help" << std::endl;
    std::cout << "\t-v | --version \t show version" << std::endl;
    std::cout << std::endl;
    std::cout << "if there are any bugs, ";
    std::cout << "please contact playinf@stu.xmu.edu.cn" << std::endl;
}

void print_version()
{
    std::cout << "infdecoder version 0.1 beta" << std::endl;
    std::cout << std::endl;
    std::cout << "if there are any bugs, ";
    std::cout << "please contact playinf@stu.xmu.edu.cn" << std::endl;
}

void initialize_features()
{
    std::string lm_des = "language model";
    std::string word_des = "word_penalty";
    std::string trans_des = "translation probablity";
    std::string rev_trans_des = "reverse translation probablity";
    std::string lex_des = "lexical weight";
    std::string rev_lex_des = "reverse lexical weight";
    std::string extracted_rule = "extracted rules";
    std::string glue_rule = "glue rules";

    model::add_feature(lm_des, lm_feature_function);
    model::add_feature(word_des, word_penalty_feature_function);
    model::add_feature(trans_des, translation_feature_function);
    model::add_feature(rev_trans_des, translation_feature_function);
    model::add_feature(lex_des, translation_feature_function);
    model::add_feature(rev_lex_des, translation_feature_function);
    model::add_feature(extracted_rule, extracted_rule_feature_function);
    model::add_feature(glue_rule, glue_rule_feature_function);
}

void initialize_parameters()
{
    int feature_num;
    std::vector<double> lambda;
    std::string value;

    value = parameter::get_parameter("feature-number");
    feature_num = std::atoi(value.c_str());

    lambda.reserve(feature_num);

    for (int i = 0; i < feature_num; i++) {
        std::string str("weight-");

        str += '0' + i;

        const std::string& weight = parameter::get_parameter(str.c_str());
        lambda.push_back(std::atof(weight.c_str()));
    }

    model::set_weight(lambda);
    value = parameter::get_parameter("rule-beam-size");
    rt_node::set_rule_limit(std::atoi(value.c_str()));
}

void initialize_language_model()
{
    std::clock_t begin;
    std::clock_t end;
    std::clock_t elapsed_time;
    const std::string* lm_file;
    std::string value;

    lm_file = &parameter::get_parameter("language-model-file");
    value = parameter::get_parameter("language-model-order");
    lm::set_language_model_order(std::atoi(value.c_str()));
    std::cout << "loading language model from " << *lm_file << std::endl;
    begin = clock();
    lm::load(lm_file->c_str());
    end = clock();
    elapsed_time = (end - begin) / CLOCKS_PER_SEC;
    std::cout << "elapsed time: " << elapsed_time << "s" << std::endl;
}

int main(int argc, char** argv)
{
    decoder translation_system;

    if (argc == 1) {
        parameter::load_configuation_file("config.txt");
    }

    // loading parameter
    for (int i = 1; i < argc; i++) {
        std::string arg(argv[i]);

        if (arg == "-c" || arg == "--config") {
            i++;
            if (i >= argc) {
                print_help();
                return 0;
            }

            parameter::load_configuation_file(argv[i]);

        } else if (arg == "-v" || arg == "--version") {
            print_version();
            return 0;
        } else {
            print_help();
            return 0;
        }
    }

    initialize_features();
    initialize_parameters();
    initialize_language_model();
    translation_system.load_rule_table(load_rule_table);
    translation_system.translate(process_input_file);

    /* dispose resources */
    lm::unload();
}
