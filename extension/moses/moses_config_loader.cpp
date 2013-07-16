/* moses_ini_loader.cpp */
#include <cmath>
#include <string>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <rule.h>
#include <model.h>
#include <config.h>
#include <symbol.h>
#include <utility.h>
#include <parameter.h>
#include <rule_tree.h>
#include <language_model.h>

typedef std::map<std::string, std::vector<std::string>> param_map;

void moses_format_loader(void* args);
rule* create_unknow_word_rule(const symbol* sym);
float hpb_rule_heuristic_function(const rule* r);

bool is_option(const char* str)
{
    if (str == nullptr)
        return false;
    std::string opt_str(str);
    unsigned int len = opt_str.size();

    if (len > 0 && opt_str[0] != '-')
        return false;

    if (len > 1) {
        unsigned int val = opt_str.substr(1, 1).find_first_not_of("0123456789");
        if (val == 0)
            return true;
    }

    return false;
}

unsigned int find_param(const std::string& param, int argc, char** argv)
{
    const unsigned int no_param = static_cast<unsigned int>(-1);
    for (int i = 0; i < argc; i++) {
        if (std::string(argv[i]) == param) {
            if (i + 1 < argc) {
                return i + 1;
            } else {
                std::cout << "require a parameter" << std::endl;
                std::exit(-1);
            }
        }
    }

    return no_param;
}

void load_rule_from_file(const char* filename, rule_tree* table)
{
    void** arg_arr = new void*[2];

    arg_arr[0] = (void*) filename;
    arg_arr[1] = (void*) table;

    moses_format_loader((void*) arg_arr);

    std::cout << "loading " << filename << " complete" << std::endl;

    delete[] arg_arr;
}

void load_moses_model()
{
    configuration* config = configuration::get_instance();
    model* system_model = config->get_model();
    parameter* param = config->get_parameter();

    config->set_unknow_word_handler(create_unknow_word_rule);

    auto& lm_filename = param->get_string_parameter("language_model");
    auto& table_filename = param->get_string_parameter("rule_table");
    auto& glue_filename = param->get_string_parameter("special_rule_table");

    language_model* lm = new language_model();
    rule_tree* table = new rule_tree();
    rule_tree* glue_table = new rule_tree();

    table->set_id(0);
    glue_table->set_id(1);

    std::cout << "loading language model";
    lm->set_order(param->get_integer_parameter("language_model_order"));
    lm->load(lm_filename.c_str());
    lm->set_id(0);
    std::cout << " complete" << std::endl;

    std::cout << "loading rule tree" << std::endl;
    load_rule_from_file(table_filename.c_str(), table);
    load_rule_from_file(glue_filename.c_str(), glue_table);

    /* add features */

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

    /* rule tree sorting and pruning */
    unsigned int rule_limit = param->get_integer_parameter("rule_limit");

    std::cout << "sorting and pruning rule table";
    table->set_heuristic_function(hpb_rule_heuristic_function);
    table->sort(rule_limit);
    table->prune(rule_limit);

    std::cout << " complete" << std::endl;
}

void load_moses_ini(const char* filename, param_map& setting)
{
    std::string line;
    std::string param;
    std::ifstream file(filename);

    if (file.bad()) {
        std::cout << "cannot open file " << filename << std::endl;
        std::exit(-1);
    }

    while (std::getline(file, line)) {
        std::string::size_type pos = line.find_first_of('#');

        /* remove comments */
        if (pos != std::string::npos)
            line = line.substr(0, pos);

        string_trim(line, line);
        unsigned int size = line.size();

        if (!size) {
            continue;
        } else if (line[0] == '[') {
            for (unsigned int i = 0; i < size; i++) {
                if (line[i] != ']')
                    continue;
                param = line.substr(1, i - 1);
                break;
            }
        } else {
            setting[param].push_back(line);
        }
    }
}

void translate_moses_ini(param_map& setting)
{
    configuration* config = configuration::get_instance();
    parameter* param = config->get_parameter();
    std::vector<std::string> str_vec;

    /* rule table */
    std::vector<std::string>& table_vec = setting["ttable-file"];

    if (table_vec.size() != 2) {
        std::cout << "do not have enough rule table" << std::endl;
        std::exit(-1);
    }

    const std::string& ntable = table_vec[0];
    const std::string& gtable = table_vec[1];

    string_split(ntable, " ", str_vec);
    param->add_parameter("rule_table", str_vec[4]);
    str_vec.clear();
    string_split(gtable, " ", str_vec);
    param->add_parameter("special_rule_table", str_vec[4]);
    str_vec.clear();

    /* language model */
    std::vector<std::string>& lm_vec = setting["lmodel-file"];

    if (lm_vec.size() != 1) {
        std::cout << "incorrect number of language model" << std::endl;
        std::exit(-1);
    }

    string_split(lm_vec[0], " ", str_vec);
    unsigned int order = std::stoi(str_vec[2]);
    param->add_parameter("language_model_order", order);
    param->add_parameter("language_model", str_vec[3]);
    str_vec.clear();

    /* rule limit */
    std::vector<std::string>& rule_limit_vec = setting["ttable-limit"];

    if (rule_limit_vec.size() > 0) {
        unsigned int limit = std::stoi(rule_limit_vec[0]);
        param->add_parameter("rule_limit", limit);
    }

    /* thread number */
    std::vector<std::string>& threads_vec = setting["threads"];

    if (threads_vec.size() > 0) {
        unsigned int num = std::stoi(threads_vec[0]);
        param->add_parameter("thread_number", num);
    }

    /* nbest */
    std::vector<std::string>& dnbest_vec = setting["distinct-nbest"];

    if (dnbest_vec.size() > 0) {
        if (dnbest_vec[0] == "true")
            param->add_parameter("distinct_nbest", 1u);
    }

    std::vector<std::string>& nbest_vec = setting["n-best-list"];

    if (nbest_vec.size() == 2) {
        unsigned int num = std::stoi(nbest_vec[1]);
        param->add_parameter("output_nbest_file", nbest_vec[0]);
        param->add_parameter("nbest_number", num);
    }

    /* input file */
    std::vector<std::string>& input_vec = setting["input-file"];

    if (input_vec.size() > 0) {
        param->add_parameter("input_file", input_vec[0]);
    }

    /* beam search */
    std::vector<std::string>& beam_size_vec = setting["stack"];
    std::vector<std::string>& beam_threshold_vec = setting["beam-threshold"];

    if (beam_size_vec.size() > 0) {
        unsigned int beam_size = std::stoi(beam_size_vec[0]);
        param->add_parameter("beam_size", beam_size);
    }

    if (beam_threshold_vec.size() > 0) {
        float threshold = std::stof(beam_threshold_vec[0]);
        threshold = std::log(threshold);
        param->add_parameter("beam_threshold", threshold);
    }

    /* weight */
    std::vector<std::string>& lm_weight = setting["weight-l"];

    if (!lm_weight.size()) {
        std::cout << "no language model weight specified" << std::endl;
        std::exit(-1);
    }

    param->add_parameter("weight_0", std::stof(lm_weight[0]));

    std::vector<std::string>& tm_weight = setting["weight-t"];

    if (tm_weight.size() != 6) {
        std::cout << "incorrect number of translation model feature weights";
        std::cout << std::endl;
        std::exit(-1);
    }

    param->add_parameter("weight_1", std::stof(tm_weight[0]));
    param->add_parameter("weight_2", std::stof(tm_weight[1]));
    param->add_parameter("weight_3", std::stof(tm_weight[2]));
    param->add_parameter("weight_4", std::stof(tm_weight[3]));
    param->add_parameter("weight_5", std::stof(tm_weight[4]));
    param->add_parameter("weight_6", std::stof(tm_weight[5]));

    std::vector<std::string>& w_weight = setting["weight-w"];

    if (!w_weight.size()) {
        std::cout << "no word penalty weight specified" << std::endl;
        std::exit(-1);
    }

    param->add_parameter("weight_7", std::stof(w_weight[0]));

    /* weight */
    std::vector<std::string>& w_lm = setting["lm"];

    if (w_lm.size() == 1) {
        param->add_parameter("weight_0", std::stof(w_lm[0]));
    }

    std::vector<std::string>& w_tm = setting["tm"];

    if (w_tm.size() == 6) {
        param->add_parameter("weight_1", std::stof(w_tm[0]));
        param->add_parameter("weight_2", std::stof(w_tm[1]));
        param->add_parameter("weight_3", std::stof(w_tm[2]));
        param->add_parameter("weight_4", std::stof(w_tm[3]));
        param->add_parameter("weight_5", std::stof(w_tm[4]));
        param->add_parameter("weight_6", std::stof(w_tm[5]));
    }

    std::vector<std::string>& w_word = setting["w"];

    if (w_word.size() == 1) {
        param->add_parameter("weight_7", std::stof(w_word[0]));
    }
}

void moses_ini_loader(const char* filename)
{
    param_map pmap;

    load_moses_ini(filename, pmap);
    translate_moses_ini(pmap);
}

void load_moses_options(int argc, char** argv)
{
    param_map pmap;
    const unsigned int no_param = static_cast<unsigned int>(-1);
    unsigned int index1 = find_param("-f", argc, argv);
    unsigned int index2 = find_param("-config", argc, argv);
    unsigned int index = index1 & index2;

    if (index == no_param) {
        std::cout << "no configuration file" << std::endl;
        std::exit(-1);
    }

    load_moses_ini(argv[index], pmap);

    for (int i = 0; i < argc; i++) {
        if (is_option(argv[i])) {
            int index = 0;
            int start_pos = i + 1;
            std::string param(argv[i] + 1);

            while (start_pos < argc && !is_option(argv[start_pos])) {
                if (pmap[param].size() > index)
                    pmap[param][index] = argv[start_pos];
                else
                    pmap[param].push_back(argv[start_pos]);
                index++;
                start_pos++;
            }
        }
    }

    translate_moses_ini(pmap);
}
