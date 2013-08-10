/* moses_ini_loader.cpp */
#include <cmath>
#include <string>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <model.h>
#include <config.h>
#include <symbol.h>
#include <utility.h>
#include <parameter.h>

typedef std::map<std::string, std::vector<std::string>> param_map;

static bool is_option(const char* str)
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

static unsigned int find_param(const std::string& param, int argc, char** argv)
{
    const unsigned int no_param = static_cast<unsigned int>(-1);
    for (int i = 0; i < argc; i++) {
        if (std::string(argv[i]) == param) {
            if (i + 1 < argc) {
                return i + 1;
            } else {
                std::cerr << "require a parameter" << std::endl;
                std::exit(-1);
            }
        }
    }

    return no_param;
}

static void load_moses_ini(const char* filename, param_map& setting)
{
    std::string line;
    std::string param;
    std::ifstream file(filename);

    if (file.bad()) {
        std::cerr << "cannot open file " << filename << std::endl;
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

static void translate_moses_ini(param_map& setting)
{
    configuration* config = configuration::get_instance();
    parameter* param = config->get_parameter();
    std::vector<std::string> str_vec;

    /* language models */
    std::vector<std::string>& lm_vec = setting["lmodel-file"];
    unsigned int lm_num = lm_vec.size();

    if (lm_num == 0) {
        std::cerr << "no language model specified" << std::endl;
        std::exit(-1);
    }

    param->add_parameter("language_model_number", lm_num);

    for (unsigned int i = 0; i < lm_num; i++) {
        std::string& lm_str = lm_vec[i];
        std::string lm_file = "language_model_";
        std::string lm_order = "language_model_";
        unsigned int order;

        string_split(lm_str, " ", str_vec);
        order = std::stoi(str_vec[2]);
        lm_file += std::to_string(i) + "_file";
        lm_order += std::to_string(i) + "_order";
        param->add_parameter(lm_order, order);
        param->add_parameter(lm_file, str_vec[3]);
        str_vec.clear();
    }

    /* translation models */
    std::vector<std::string>& table_vec = setting["ttable-file"];
    unsigned int tm_num = table_vec.size();
    unsigned int tm_fea_num = 0;
    param->add_parameter("translation_model_number", tm_num);

    if (tm_num < 2) {
        std::cerr << "do not have enough rule table" << std::endl;
        std::exit(-1);
    }

    for (unsigned int i = 0; i < tm_num; i++) {
        std::string& tm = table_vec[i];
        std::string tm_file = "translation_model_";
        std::string tm_fnum = "translation_model_";
        unsigned int tm_feature_number;

        string_split(tm, " ", str_vec);
        tm_feature_number = std::stoi(str_vec[3]);
        tm_file += std::to_string(i) + "_file";
        tm_fnum += std::to_string(i) + "_feature_number";
        param->add_parameter(tm_fnum, tm_feature_number);
        param->add_parameter(tm_file, str_vec[4]);
        tm_fea_num += tm_feature_number;
        str_vec.clear();
    }

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

    /* cube pruning */
    std::vector<std::string>& pop_lim_vec = setting["cube-pruning-pop-limit"];

    if (pop_lim_vec.size() > 0) {
        unsigned int pop_limit = std::stoi(pop_lim_vec[0]);
        param->add_parameter("pop_limit", pop_limit);
    }

    /* weight */
    unsigned int feature_id = 0;
    std::vector<std::string>& lm_weight1 = setting["weight-l"];
    std::vector<std::string>& lm_weight2 = setting["lm"];
    std::vector<std::string>* lm_weight_ptr;
    std::vector<std::string>& tm_weight1 = setting["weight-t"];
    std::vector<std::string>& tm_weight2 = setting["tm"];
    std::vector<std::string>* tm_weight_ptr;
    std::vector<std::string>& w_weight1 = setting["weight-w"];
    std::vector<std::string>& w_weight2 = setting["w"];
    std::vector<std::string>* w_weight_ptr;

    if (lm_weight2.size() > 0)
        lm_weight_ptr = &lm_weight2;
    else
        lm_weight_ptr = &lm_weight1;

    if (tm_weight2.size() > 0)
        tm_weight_ptr = &tm_weight2;
    else
        tm_weight_ptr = &tm_weight1;

    if (w_weight2.size() > 0)
        w_weight_ptr = &w_weight2;
    else
        w_weight_ptr = &w_weight1;

    /* language model weights */
    std::vector<std::string>& lm_weight = *lm_weight_ptr;
    unsigned int lm_weight_num = lm_weight.size();

    if (lm_weight_num != lm_num) {
        std::cerr << "incorrect number of language model weights";
        std::cerr << std::endl;
        std::exit(-1);
    }

    for (unsigned int i = 0; i < lm_weight_num; i++) {
        std::string weight_str = "weight_" + std::to_string(feature_id);
        param->add_parameter(weight_str, std::stof(lm_weight[i]));
        feature_id++;
    }

    /* translation model weights*/
    std::vector<std::string>& tm_weight = *tm_weight_ptr;
    unsigned int tm_weight_num = tm_weight.size();

    if (tm_weight_num != tm_fea_num) {
        std::cerr << "incorrect number of translation model weights";
        std::cerr << std::endl;
        std::exit(-1);
    }

    for (unsigned int i = 0; i < tm_weight_num; i++) {
        std::string weight_str = "weight_" + std::to_string(feature_id);
        param->add_parameter(weight_str, std::stof(tm_weight[i]));
        feature_id++;
    }

    /* word penalty weight */
    std::vector<std::string>& w_weight = *w_weight_ptr;
    std::string weight_str = "weight_" + std::to_string(feature_id);

    if (!w_weight.size()) {
        std::cerr << "no word penalty weight specified" << std::endl;
        std::exit(-1);
    }

    param->add_parameter(weight_str, std::stof(w_weight[0]));

    if (setting.find("show-weights") != setting.end())
        param->add_parameter("show_weights", 1u);

    if (setting.find("drop-unknow") != setting.end()) {
        if (setting["drop-unknow"][0] == "true") {
            param->add_parameter("drop_unknow_word", 1u);
        }
    }

    if (setting.find("tense-dict") != setting.end())
        param->add_parameter("tense_dict", setting["tense-dict"][0]);

    param->add_parameter("feature_number", lm_num + tm_fea_num + 1);
}

void load_moses_options(int argc, char** argv)
{
    param_map pmap;
    const unsigned int no_param = static_cast<unsigned int>(-1);
    unsigned int index1 = find_param("-f", argc, argv);
    unsigned int index2 = find_param("-config", argc, argv);
    unsigned int index = index1 & index2;

    if (index == no_param) {
        std::cerr << "no configuration file" << std::endl;
        std::exit(-1);
    }

    load_moses_ini(argv[index], pmap);

    for (int i = 0; i < argc; i++) {
        if (is_option(argv[i])) {
            int index = 0;
            int start_pos = i + 1;
            std::string param(argv[i] + 1);

            pmap[param];

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
