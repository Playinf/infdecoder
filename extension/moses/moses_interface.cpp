/* moses_interface.cpp */
#include <string>
#include <cstdlib>
#include <utility>
#include <iostream>
#include <model.h>
#include <config.h>
#include <parameter.h>

/* defined in moses_model.cpp */
void load_moses_model();
/* defined in moses_config_loader.cpp */
void load_moses_options(int argc, char** argv);
/* defined in standard.cpp */
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

    float lm_weight = system_model->get_weight(0);
    std::cout << "LM lm " << lm_weight << std::endl;

    for (unsigned int i = 1; i < 6; i++) {
        float weight = system_model->get_weight(i);
        std::cout << "PhraseModel tm " << weight << std::endl;
    }

    float g_weight = system_model->get_weight(6);
    std::cout << "PhraseModel:2 tm " << g_weight << std::endl;

    float w_weight = system_model->get_weight(7);
    std::cout << "WordPenalty w " << w_weight << std::endl;
}

void moses_translate(int argc, char** argv)
{
    configuration* config = configuration::get_instance();
    parameter* param = config->get_parameter();

    load_moses_options(argc, argv);
    config->load_parameter();
    config->load_weight();

    if (param->get_integer_parameter("show_weights")) {
        show_weights();
        std::exit(0);
    }

    print_parameter();
    load_moses_model();
    translate();

    std::cerr << "translation complete" << std::endl;
}
