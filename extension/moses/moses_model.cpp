/* moses_model.cpp */
#include <string>
#include <iostream>
#include <stdexcept>
#include <innlm.h>
#include <innjm.h>
#include <model.h>
#include <srilm.h>
#include <config.h>
#include <innltm.h>
#include <parameter.h>
#include <rule_tree.h>
#include <joint_model.h>
#include <penalty_model.h>
#include <language_model.h>
#include <feature_function.h>
#include <rescore_function.h>
#include <translation_model.h>
#include <ngram_language_model.h>

// defined in moses_rule_loader.cpp
void moses_format_loader(void* args);
// defined in moses_oov_rule.cpp
rule* create_unknown_word_rule(const symbol* sym);
rule* create_null_word_rule(const symbol* sym);
// defined in rule_heuristic.cpp
float hpb_rule_heuristic_function(const rule* r);
// defined in moses_rule_heuristic.cpp
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
    configuration* config = configuration::get_instance();
    model* system_model = config->get_model();
    parameter* param = config->get_parameter();

    if (param->get_integer_parameter("drop_unknown_word"))
        config->set_unknown_word_handler(create_null_word_rule);
    else
        config->set_unknown_word_handler(create_unknown_word_rule);

    // language models
    unsigned int lm_num;
    lm_num = param->get_integer_parameter("language_model_number");

    for (unsigned int i = 0; i < lm_num; i++) {
        joint_model* jm;
        lexical_model* lexm;
        language_model* lm;
        ngram_language_model* nlm;
        std::string feature_name = "langauge model ";
        std::string feature_description = "language model";
        std::string index = std::to_string(i);
        std::string lm_impl_name = "language_model_";
        std::string lm_mode_name = "language_model_";
        std::string lm_order_param = "language_model_";
        std::string sctx_str = "joint_model_";
        std::string tctx_str = "joint_model_";
        std::string win_str = "lexical_model_";
        unsigned int impl_id;
        unsigned int lm_order;
        unsigned int src_context;
        unsigned int tgt_context;
        unsigned int win_size;
        feature_function func = language_model_feature_function;

        win_str += std::to_string(i) + "_window_size";
        sctx_str += std::to_string(i) + "_source_context";
        tctx_str += std::to_string(i) + "_target_context";

        lm_impl_name += std::to_string(i) + "_implementation";
        lm_mode_name += std::to_string(i) + "_mode";
        lm_order_param += index + "_order";

        impl_id = param->get_integer_parameter(lm_impl_name);

        switch (impl_id) {
        case 0:
            nlm = new srilm();
            func = language_model_feature_function;
            lm_order = param->get_integer_parameter(lm_order_param);
            nlm->set_order(lm_order);
            nlm->set_type(language_model_type::forward);
            lm = nlm;
            break;
        case 1:
            nlm = new srilm();
            func = language_model_feature_function;
            lm_order = param->get_integer_parameter(lm_order_param);
            nlm->set_order(lm_order);
            nlm->set_type(language_model_type::backward);
            lm = nlm;
            break;
        case 2:
            nlm = new innlm();
            func = language_model_feature_function;
            lm_order = param->get_integer_parameter(lm_order_param);
            nlm->set_order(lm_order);
            nlm->set_type(language_model_type::forward);
            lm = nlm;
            break;
        case 3:
            nlm = new innlm();
            func = language_model_feature_function;
            lm_order = param->get_integer_parameter(lm_order_param);
            nlm->set_order(lm_order);
            nlm->set_type(language_model_type::backward);
            lm = nlm;
            break;
        case 4:
            jm = new innjm();
            func = joint_model_feature_function;
            src_context = param->get_integer_parameter(sctx_str);
            tgt_context = param->get_integer_parameter(tctx_str);
            jm->set_source_context(src_context);
            jm->set_target_context(tgt_context);
            jm->set_type(language_model_type::forward);
            jm->set_model_type(joint_model_type::source_to_target);
            lm = jm;
            break;
        case 5:
            jm = new innjm();
            func = joint_model_feature_function;
            src_context = param->get_integer_parameter(sctx_str);
            tgt_context = param->get_integer_parameter(tctx_str);
            jm->set_source_context(src_context);
            jm->set_target_context(tgt_context);
            jm->set_type(language_model_type::forward);
            jm->set_model_type(joint_model_type::source_to_target);
            lm = jm;
            break;
        case 10:
            nlm = new srilm();
            func = language_model_rescore_function;
            lm_order = param->get_integer_parameter(lm_order_param);
            nlm->set_order(lm_order);
            nlm->set_type(language_model_type::forward);
            lm = nlm;
            break;
        case 11:
            nlm = new srilm();
            func = language_model_rescore_function;
            lm_order = param->get_integer_parameter(lm_order_param);
            nlm->set_order(lm_order);
            nlm->set_type(language_model_type::backward);
            lm = nlm;
            break;
        case 12:
            nlm = new innlm();
            func = language_model_rescore_function;
            lm_order = param->get_integer_parameter(lm_order_param);
            nlm->set_order(lm_order);
            nlm->set_type(language_model_type::forward);
            lm = nlm;
            break;
        case 13:
            nlm = new innlm();
            func = language_model_rescore_function;
            lm_order = param->get_integer_parameter(lm_order_param);
            nlm->set_order(lm_order);
            nlm->set_type(language_model_type::backward);
            lm = nlm;
            break;
        case 14:
            jm = new innjm();
            func = joint_model_rescore_function;
            src_context = param->get_integer_parameter(sctx_str);
            tgt_context = param->get_integer_parameter(tctx_str);
            jm->set_source_context(src_context);
            jm->set_target_context(tgt_context);
            jm->set_type(language_model_type::forward);
            jm->set_model_type(joint_model_type::source_to_target);
            lm = jm;
            break;
        case 15:
            jm = new innjm();
            func = joint_model_rescore_function;
            src_context = param->get_integer_parameter(sctx_str);
            tgt_context = param->get_integer_parameter(tctx_str);
            jm->set_source_context(src_context);
            jm->set_target_context(tgt_context);
            jm->set_type(language_model_type::backward);
            jm->set_model_type(joint_model_type::source_to_target);
            lm = jm;
            break;
        case 16:
            jm = new innjm();
            func = joint_model_rescore_function;
            src_context = param->get_integer_parameter(sctx_str);
            tgt_context = param->get_integer_parameter(tctx_str);
            jm->set_source_context(src_context);
            jm->set_target_context(tgt_context);
            jm->set_type(language_model_type::forward);
            jm->set_model_type(joint_model_type::target_to_source);
            lm = jm;
            break;
        case 17:
            jm = new innjm();
            func = joint_model_rescore_function;
            src_context = param->get_integer_parameter(sctx_str);
            tgt_context = param->get_integer_parameter(tctx_str);
            jm->set_source_context(src_context);
            jm->set_target_context(tgt_context);
            jm->set_type(language_model_type::backward);
            jm->set_model_type(joint_model_type::target_to_source);
            lm = jm;
            break;
        case 20:
            lexm = new innltm();
            func = lexical_model_feature_function;
            win_size = param->get_integer_parameter(win_str);
            lexm->set_window_size(win_size);
            lexm->set_model_type(lexical_model_type::source_to_target);
            lm = lexm;
            break;
        case 21:
            lexm = new innltm();
            func = lexical_model_rescore_function;
            win_size = param->get_integer_parameter(win_str);
            lexm->set_window_size(win_size);
            lexm->set_model_type(lexical_model_type::source_to_target);
            lm = lexm;
        case 22:
            lexm = new innltm();
            func = lexical_model_rescore_function;
            win_size = param->get_integer_parameter(win_str);
            lexm->set_window_size(win_size);
            lexm->set_model_type(lexical_model_type::target_to_source);
        default:
            throw std::runtime_error("unrecognized language model");
        }

        feature_name += index;
        lm->set_id(i);
        id = system_model->add_feature(feature_name, feature_description);
        lm->set_feature(id);
        system_model->add_language_model(lm);
        system_model->set_feature_function(id, func);
        system_model->set_feature_source(id, lm);
    }

    // translation models
    unsigned int tm_num;
    tm_num = param->get_integer_parameter("translation_model_number");

    for (unsigned int i = 0; i < tm_num; i++) {
        translation_model* tm;
        rule_translation_model* rtm = new rule_translation_model;
        rule_tree* tree = new rule_tree();
        std::string index = std::to_string(i);
        std::string tm_fea_name = "translation model " + index;
        std::string tm_fnum_param = "translation_model_" + index;

        tm = rtm;

        tm_fnum_param += "_feature_number";
        unsigned int fnum = param->get_integer_parameter(tm_fnum_param);
        tm->set_id(i);
        tree->set_id(i);
        rtm->add_rule_tree(tree);

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

    // word penalty
    word_penalty_model* w_model = new word_penalty_model();
    w_model->set_id(0);
    w_model->set_penalty(-0.434294482f);
    id = system_model->add_feature("word penalty", "build in word penalty");
    system_model->set_feature_function(id, word_penalty_feature_function);
    system_model->add_word_penalty_model(w_model);

    // unknown word penalty
    translation_model* tm_unknow = new translation_model();
    id = system_model->add_feature("unknown word penalty", "oov penalty");
    system_model->add_translation_model(tm_unknow);
    system_model->set_feature_function(id, unknow_word_feature_function);
    system_model->push_weight(1.0f);
    tm_unknow->add_feature(id);
}

void load_model_from_file()
{
    configuration* config = configuration::get_instance();
    model* system_model = config->get_model();
    parameter* param = config->get_parameter();
    unsigned int lm_num;
    unsigned int tm_num;

    lm_num = param->get_integer_parameter("language_model_number");
    tm_num = param->get_integer_parameter("translation_model_number");

    // load language models
    for (unsigned int i = 0; i < lm_num; i++) {
        language_model* lm = system_model->get_language_model(i);
        std::string lm_file_param = "language_model_";
        std::string index = std::to_string(i);

        lm_file_param += index + "_file";
        auto& lm_file = param->get_string_parameter(lm_file_param);
        std::cerr << "loading language model from " << lm_file << std::endl;
        lm->load(lm_file.c_str());
    }

    // load translation models
    unsigned int rule_limit = param->get_integer_parameter("rule_limit");

    for (unsigned int i = 0; i < tm_num; i++) {
        translation_model* tm = system_model->get_translation_model(i);
        rule_translation_model* rtm = static_cast<rule_translation_model*>(tm);
        rule_tree* tree = rtm->get_rule_tree();
        std::string index = std::to_string(i);
        std::string tm_file_param = "translation_model_" + index;

        tm_file_param += "_file";
        auto& tm_file = param->get_string_parameter(tm_file_param);
        std::cerr << "loading rule tree from " << tm_file << std::endl;
        load_rule_from_file(tm_file.c_str(), tree);
        std::cerr << "complete" << std::endl;
        std::cerr << "sorting and pruning rule table";
        tree->set_heuristic_function(moses_rule_heuristic_function);
        tree->sort_and_prune(rule_limit);
        std::cerr << " complete" << std::endl;
    }
}
