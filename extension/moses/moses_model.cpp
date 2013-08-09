/* moses_model.cpp */
#include <string>
#include <iostream>
#include <model.h>
#include <config.h>
#include <parameter.h>
#include <rule_tree.h>
#include <penalty_model.h>
#include <language_model.h>
#include <translation_model.h>

/* defined in moses_rule_loader.cpp */
void moses_format_loader(void* args);
/* defined in moses_oov_rule.cpp */
rule* create_unknow_word_rule(const symbol* sym);
/* defined in rule_heuristic.cpp */
float hpb_rule_heuristic_function(const rule* r);
/* defined in moses_rule_heuristic.cpp */
float moses_rule_heuristic_function(const rule* r);

static void load_rule_from_file(const char* filename, rule_tree* table)
{
    void** arg_arr = new void*[2];

    arg_arr[0] = (void*) filename;
    arg_arr[1] = (void*) table;

    moses_format_loader((void*) arg_arr);

    delete[] arg_arr;
}

void load_moses_model()
{
    unsigned int id;
    unsigned int rule_limit;
    configuration* config = configuration::get_instance();
    model* system_model = config->get_model();
    parameter* param = config->get_parameter();

    config->set_unknow_word_handler(create_unknow_word_rule);

    /* language models */
    unsigned int lm_num = param->get_integer_parameter("language_model_number");

    for (unsigned int i = 0; i < lm_num; i++) {
        language_model* lm = new language_model();
        std::string lm_feature_name = "langauge model ";
        std::string lm_feature_description = "language model";
        std::string index = std::to_string(i);

        lm_feature_name += index;
        lm->set_id(i);
        id = system_model->add_feature(lm_feature_name, lm_feature_description);
        lm->set_feature(id);
        system_model->add_language_model(lm);
        system_model->set_feature_function(id, language_model_feature_function);
        system_model->set_feature_source(id, lm);
    }

    /* translation models */
    unsigned int tm_num;
    tm_num = param->get_integer_parameter("translation_model_number");

    for (unsigned int i = 0; i < tm_num; i++) {
        translation_model* tm = new translation_model();
        rule_tree* tree = new rule_tree();
        std::string index = std::to_string(i);
        std::string tm_fea_name = "translation model " + index;
        std::string tm_fnum_param = "translation_model_" + index;

        tm_fnum_param += "_feature_number";
        unsigned int fnum = param->get_integer_parameter(tm_fnum_param);
        tm->set_id(i);
        tree->set_id(i);
        tm->add_rule_tree(tree);

        for (unsigned int j = 0; j < fnum; j++) {
            std::string index = std::to_string(j);
            std::string des = "translation model";
            std::string fea_name = tm_fea_name + " feature " + index;
            auto func = translation_model_feature_function;
            id = system_model->add_feature(fea_name, des);
            system_model->set_feature_function(id, func);
            system_model->set_feature_source(id, tm);
            tm->add_feature(id);
        }

        system_model->add_translation_model(tm);
    }

    /* word penalty */
    word_penalty_model* w_model = new word_penalty_model();
    w_model->set_id(0);
    w_model->set_penalty(-0.434294482f);
    id = system_model->add_feature("word penalty", "build in word penalty");
    system_model->set_feature_function(id, word_penalty_feature_function);
    system_model->add_word_penalty_model(w_model);

    /* unkonw word penalty */
    translation_model* tm_unknow = new translation_model();
    id = system_model->add_feature("unknow word penalty", "oov penalty");
    system_model->add_translation_model(tm_unknow);
    system_model->set_feature_function(id, unknow_word_feature_function);
    system_model->push_weight(1.0f);
    tm_unknow->add_feature(id);

    /* load language models */
    for (unsigned int i = 0; i < lm_num; i++) {
        language_model* lm = system_model->get_language_model(i);
        std::string lm_file_param = "language_model_";
        std::string lm_order_param = "language_model_";
        std::string index = std::to_string(i);

        lm_file_param += index + "_file";
        lm_order_param += index + "_order";
        auto& lm_file = param->get_string_parameter(lm_file_param);
        unsigned int lm_order = param->get_integer_parameter(lm_order_param);
        lm->set_order(lm_order);
        std::cerr << "loading language model from " << lm_file << std::endl;
        lm->load(lm_file.c_str());
    }

    /* load translation models */
    rule_limit = param->get_integer_parameter("rule_limit");

    for (unsigned int i = 0; i < tm_num; i++) {
        translation_model* tm = system_model->get_translation_model(i);
        rule_tree* tree = tm->get_rule_tree();
        std::string index = std::to_string(i);
        std::string tm_file_param = "translation_model_" + index;

        tm_file_param += "_file";
        auto& tm_file = param->get_string_parameter(tm_file_param);
        std::cerr << "loading rule tree from" << tm_file;
        load_rule_from_file(tm_file.c_str(), tree);
        std::cerr << " complete" << std::endl;
        std::cerr << "sorting and pruning rule table";
        tree->set_heuristic_function(moses_rule_heuristic_function);
        tree->sort_and_prune(rule_limit);
        std::cerr << " complete" << std::endl;
    }
}
