/* moses_interface.cpp */
#include <string>
#include <cstdlib>
#include <utility>
#include <iostream>
#include <model.h>
#include <config.h>
#include <parameter.h>
#include <translation_model.h>

// defined in moses_model.cpp
void load_moses_model();
void load_model_from_file();
// defined in moses_config_loader.cpp
void load_moses_options(int argc, char** argv);
// defined in standard.cpp
void translate();
void singel_thread_translate();

static void print_parameter()
{
    configuration* config = configuration::get_instance();
    parameter* param = config->get_parameter();
    std::vector<std::pair<std::string, std::string>> vec;

    param->get_parameter(vec);

    for (auto& val : vec) {
        std::cerr << val.first << ":" << val.second << std::endl;
    }
}

static void show_weights()
{
    configuration* config = configuration::get_instance();
    model* system_model = config->get_model();
    unsigned int lm_num = system_model->get_language_model_number();
    unsigned int tm_num = system_model->get_translation_model_number();
    unsigned int id = 0;

    float lm_weight = system_model->get_weight(id++);
    std::cout << "LM lm " << lm_weight << std::endl;

    for (unsigned int i = 1; i < lm_num; i++) {
        float weight = system_model->get_weight(id++);
        std::cout << "LM:" << i + 1 << " lm " << weight << std::endl;
    }

    translation_model* tm = system_model->get_translation_model(0);
    unsigned int fea_num = tm->get_feature_number();

    for (unsigned int i = 0; i < fea_num; i++) {
        float weight = system_model->get_weight(id++);
        std::cout << "PhraseModel tm " << weight << std::endl;
    }

    for (unsigned int i = 1; i < tm_num - 1; i++) {
        translation_model* tm = system_model->get_translation_model(i);
        unsigned int fea_num = tm->get_feature_number();

        for (unsigned int j = 0; j < fea_num; j++) {
            float weight = system_model->get_weight(id++);
            std::cout << "PhraseModel:" << i + 1 << " tm ";
            std::cout << weight << std::endl;
        }
    }

    float w_weight = system_model->get_weight(id++);
    std::cout << "WordPenalty w " << w_weight << std::endl;
}

void moses_translate(int argc, char** argv)
{
    configuration* config = configuration::get_instance();
    parameter* param = config->get_parameter();

    load_moses_options(argc, argv);
    config->load_parameter();
    config->load_weight();
    load_moses_model();

    if (param->get_integer_parameter("show_weights")) {
        show_weights();
        std::exit(0);
    }

    print_parameter();
    load_model_from_file();
    translate();

    std::cerr << "translation complete" << std::endl;
}
