/* decoder.cpp */
#include <ctime>
#include <string>
#include <vector>
#include <lexer.h>
#include <model.h>
#include <config.h>
#include <parser.h>
#include <decoder.h>
#include <trellis.h>
#include <utility.h>
#include <rule_tree.h>
#include <information.h>
#include <translation_model.h>

decoder::decoder()
{
    info = nullptr;
    lexical_analyzer = nullptr;
    syntax_analyzer = nullptr;
}

decoder::~decoder()
{
    delete info;
    delete lexical_analyzer;
    delete syntax_analyzer;
}

hypothesis* decoder::get_best_hypothesis() const
{
    return syntax_analyzer->get_best_hypothesis();
}

void decoder::get_nbest_list(path_vector& vec) const
{
    configuration* config = configuration::get_instance();
    unsigned int nbest_number = config->get_nbest_number();

    syntax_analyzer->get_nbest(nbest_number, &vec);
}

void decoder::process(const std::string& sentence)
{
    const std::string seperator = " ";
    std::vector<std::string> word_seq;
    std::vector<rule_tree*> tree_vec;
    configuration* config = configuration::get_instance();
    model* system_model = config->get_model();
    unknown_word_handler handler;
    unsigned int tm_number = system_model->get_translation_model_number();

    string_split(sentence, seperator, word_seq);
    handler = config->get_unknown_word_handler();

    lexical_analyzer = new lexer();
    lexical_analyzer->set_input(word_seq);
    lexical_analyzer->set_handler(handler);
    lexical_analyzer->process();

    rule_tree* unknow_word_tree = lexical_analyzer->get_rule_tree();
    tree_vec.push_back(unknow_word_tree);

    for (unsigned int i = 0; i < tm_number - 1; i++) {
        translation_model* tm = system_model->get_translation_model(i);
        rule_translation_model* rtm;

        rtm = static_cast<rule_translation_model*>(tm);

        if (rtm == nullptr)
            continue;

        rule_tree* tree = rtm->get_rule_tree();
        tree_vec.push_back(tree);
    }

    info = new information();
    info->set_sentence(&lexical_analyzer->get_output());
    syntax_analyzer = new parser(&tree_vec);
    syntax_analyzer->parse(info);
}
