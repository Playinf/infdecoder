/* moses_model.cpp */
#include <string>
#include <iostream>
#include <model.h>
#include <config.h>
#include <parameter.h>
#include <rule_tree.h>
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

    std::cerr << "loading " << filename << " complete" << std::endl;

    delete[] arg_arr;
}

void load_moses_model()
{
    unsigned int id;
    configuration* config = configuration::get_instance();
    model* system_model = config->get_model();
    parameter* param = config->get_parameter();

    config->set_unknow_word_handler(create_unknow_word_rule);

    /* language model */
    language_model* lm = new language_model();

    lm->set_id(0);
    id = system_model->add_feature("language model", "main language model");
    system_model->add_language_model(lm);
    system_model->set_feature_function(id, language_model_feature_function);
    system_model->set_feature_source(id, lm);
    lm->set_feature(id);

    auto& lm_filename = param->get_string_parameter("language_model");
    std::cerr << "loading language model";
    lm->set_order(param->get_integer_parameter("language_model_order"));
    lm->load(lm_filename.c_str());

    /* translation model */
    rule_tree* table = new rule_tree();
    rule_tree* glue_table = new rule_tree();
    translation_model* tm_main = new translation_model();
    translation_model* tm_glue = new translation_model();
    translation_model* tm_unknow = new translation_model();

    tm_unknow->set_id(0);
    tm_main->set_id(1);
    tm_glue->set_id(2);
    table->set_id(1);
    glue_table->set_id(2);

    id = system_model->add_feature("inverse translation probability", "rule");
    system_model->set_feature_function(id, translation_model_feature_function);
    system_model->set_feature_source(id, tm_main);
    tm_main->add_feature(id);
    id = system_model->add_feature("inverse lexical weight", "rule");
    system_model->set_feature_function(id, translation_model_feature_function);
    system_model->set_feature_source(id, tm_main);
    tm_main->add_feature(id);
    id = system_model->add_feature("translation probability", "rule");
    system_model->set_feature_function(id, translation_model_feature_function);
    system_model->set_feature_source(id, tm_main);
    tm_main->add_feature(id);
    id = system_model->add_feature("lexical weight", "rule");
    system_model->set_feature_function(id, translation_model_feature_function);
    system_model->set_feature_source(id, tm_main);
    tm_main->add_feature(id);
    id = system_model->add_feature("rule penalty", "rule");
    system_model->set_feature_function(id, translation_model_feature_function);
    system_model->set_feature_source(id, tm_main);
    tm_main->add_feature(id);
    id = system_model->add_feature("glue rule penalty", "glue grammar");
    system_model->set_feature_function(id, translation_model_feature_function);
    system_model->set_feature_source(id, tm_glue);
    tm_glue->add_feature(id);

    tm_main->add_rule_tree(table);
    tm_glue->add_rule_tree(glue_table);

    system_model->add_translation_model(tm_unknow);
    system_model->add_translation_model(tm_main);
    system_model->add_translation_model(tm_glue);

    auto& table_filename = param->get_string_parameter("rule_table");
    auto& glue_filename = param->get_string_parameter("special_rule_table");

    std::cerr << "loading rule tree" << std::endl;
    load_rule_from_file(table_filename.c_str(), table);
    load_rule_from_file(glue_filename.c_str(), glue_table);
    std::cerr << " complete" << std::endl;

    /* word penalty */
    id = system_model->add_feature("word penalty", "build in word penalty");
    system_model->set_feature_function(id, word_penalty_feature_function);

    /* unkonw word penalty */
    id = system_model->add_feature("unknow word penalty", "oov penalty");
    system_model->set_feature_function(id, unknow_word_feature_function);
    system_model->push_weight(1.0f);
    tm_unknow->add_feature(id);

    /* rule tree sorting and pruning */
    unsigned int rule_limit = param->get_integer_parameter("rule_limit");

    std::cerr << "sorting and pruning rule table";
    table->set_heuristic_function(moses_rule_heuristic_function);
    table->sort(rule_limit);
    table->prune(rule_limit);

    std::cerr << " complete" << std::endl;
}
