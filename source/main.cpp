/* main.cpp */
#include <ctime>
#include <string>
#include <cstdlib>
#include <fstream>
#include <sstream>
#include <utility>
#include <iostream>
#include <task.h>
#include <rule.h>
#include <model.h>
#include <buffer.h>
#include <config.h>
#include <format.h>
#include <symbol.h>
#include <trellis.h>
#include <decoder.h>
#include <utility.h>
#include <rule_set.h>
#include <parameter.h>
#include <hypothesis.h>
#include <language_model.h>
#include <feature_function.h>

#include <verbose.h>

/* TODO:
 * version 0.1
 * 1. add nbest support (finished)
 * 2. add multi-thread support (finished)
 * 3. add dynamic linked feature function
 * 4. add dynamic linked io function
 */

/* moses extensions */
void load_moses_model();
void moses_ini_loader(const char* filename);
rule* create_unknow_word_rule(const symbol* sym);
void load_moses_options(int argc, char** argv);

void print_help()
{
    std::cout << "infdecoder version 0.1" << std::endl;
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
    std::cout << "infdecoder version 0.1" << std::endl;
    std::cout << std::endl;
    std::cout << "if there are any bugs, ";
    std::cout << "please contact playinf@stu.xmu.edu.cn" << std::endl;
}

void print_parameter()
{
    configuration* config = configuration::get_instance();
    parameter* param = config->get_parameter();
    std::vector<std::pair<std::string, std::string>> vec;

    param->get_parameter(vec);

    for (auto& val : vec) {
        std::cerr << val.first << ":" << val.second << std::endl;
    }
}

void show_weights()
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

void output_hypothesis(const hypothesis* h, std::vector<const std::string*>& s)
{
    const rule* target_rule = h->get_rule();
    auto& rule_body = target_rule->get_target_rule_body();
    unsigned int size = rule_body.size();
    unsigned int nonterminal_index = 0;

    for (unsigned int i = 0; i < size; ++i) {
        const symbol* sym = rule_body[i];

        if (sym->is_terminal()) {
            s.push_back(sym->get_name());
        } else {
            hypothesis *hypo = h->get_previous_hypothesis(nonterminal_index);
            ++nonterminal_index;
            output_hypothesis(hypo, s);
        }
    }
}

void output_hypothesis(const hypothesis* hypo, std::string& out)
{
    std::vector<const std::string*> s;
    std::stringstream string_stream;

    output_hypothesis(hypo, s);

    for (unsigned int i = 1; i < s.size() - 1; i++) {
        string_stream << *s[i];
        string_stream << " ";
    }

    string_stream << std::endl;
    out = string_stream.str();
}

void output_nbest(const trellis_path* path, std::string& out)
{
    std::stringstream string_stream;
    std::vector<const std::string*> output_vec;

    path->output(&output_vec);

    for (unsigned int i = 1; i < output_vec.size() - 1; i++) {
        const std::string& str = *output_vec[i];
        string_stream << str << " ";
    }

    auto score_vec = path->get_score_vector();
    string_stream << " ||| ";

    for (unsigned int i = 0; i < score_vec->size(); i++) {
        if (i == 0)
            string_stream << "lm: ";
        else if (i == 7)
            string_stream << "w: ";
        else if (i == 1)
            string_stream << "tm: ";
        string_stream << std::to_string(score_vec->at(i));
        string_stream << " ";
    }

    string_stream << " ||| ";
    string_stream << std::to_string(path->get_total_score());
    out = string_stream.str();
}

void translate()
{
    std::string line;
    unsigned int id = 0;
    std::ifstream file;
    std::ofstream nbest_file;
    std::istream* stream;
    configuration* config = configuration::get_instance();
    parameter* param = config->get_parameter();
    std::string input_file_name = param->get_string_parameter("input_file");
    std::string nbest_name = param->get_string_parameter("output_nbest_file");
    unsigned int thread_number = param->get_integer_parameter("thread_number");
    task_manager manager(thread_number);

    file.open(input_file_name);
    nbest_file.open(nbest_name);

    manager.set_limit(thread_number);

    if (file.fail())
        stream = &std::cin;
    else
        stream = &file;

    output_buffer translation_buffer(&std::cout);
    output_buffer nbest_buffer(&nbest_file);
    output_buffer* nbest_buffer_ptr;

    if (nbest_file.fail())
        nbest_buffer_ptr = nullptr;
    else
        nbest_buffer_ptr = &nbest_buffer;

    while (std::getline(*stream, line)) {
        unsigned int line_size = line.size();
        std::string::size_type pos1;
        std::string::size_type pos2;

        if (line.empty())
            continue;

        pos1 = line.find('\n');
        pos2 = line.find('\r');

        if (pos1 != line.npos) {
            line[pos1] = ' ';
        }

        if (pos2 != line.npos) {
            line[pos2] = ' ';
        }

        task* t = new task();
        t->set_id(id++);
        t->set_input(line);
        t->set_nbest_buffer(nbest_buffer_ptr);
        t->set_translation_buffer(&translation_buffer);
        t->set_handler(output_hypothesis);
        t->set_handler(output_nbest);
        manager.add_task(t);
    }

    manager.stop();
}

void single_thread_translate()
{
    std::string line;
    unsigned int id = 0;
    std::ifstream file;
    std::ofstream nbest_file;
    std::istream* stream;
    configuration* config = configuration::get_instance();
    parameter* param = config->get_parameter();
    std::string input_file_name = param->get_string_parameter("input_file");
    std::string nbest_name = param->get_string_parameter("output_nbest_file");

    file.open(input_file_name);
    nbest_file.open(nbest_name);

    if (file.fail())
        stream = &std::cin;
    else
        stream = &file;

    output_buffer translation_buffer(&std::cout);
    output_buffer nbest_buffer(&nbest_file);
    output_buffer* nbest_buffer_ptr;

    if (nbest_file.fail())
        nbest_buffer_ptr = nullptr;
    else
        nbest_buffer_ptr = &nbest_buffer;

    while (std::getline(*stream, line)) {
        unsigned int line_size = line.size();
        std::string::size_type pos1;
        std::string::size_type pos2;

        pos1 = line.find('\n');
        pos2 = line.find('\r');

        if (pos1 != line.npos) {
            line[pos1] = ' ';
        }

        if (pos2 != line.npos) {
            line[pos2] = ' ';
        }

        decoder translator;

        translator.process(line);
        hypothesis* best_hypo = translator.get_best_hypothesis();

        if (best_hypo == nullptr)
            continue;

        std::string hypo_output;

        output_hypothesis(best_hypo, hypo_output);
        translation_buffer.write(hypo_output, id);

        if (nbest_buffer_ptr == nullptr) {
            id++;
            continue;
        }

        std::vector<std::shared_ptr<const trellis_path>> path_list;
        translator.get_nbest_list(path_list);
        unsigned int size = path_list.size();
        std::stringstream out_stream;

        for (unsigned int i = 0; i < size; i++) {
            auto& path = path_list[i];
            std::string nbest_str;
            out_stream << std::to_string(id);
            out_stream << " ||| ";
            output_nbest(path.get(), nbest_str);
            out_stream << nbest_str << std::endl;
        }

        out_stream << std::flush;

        nbest_buffer_ptr->write(out_stream.str(), id);
        id++;
    }
}

int main(int argc, char** argv)
{
    configuration* config = configuration::get_instance();
    parameter* param = config->get_parameter();
    model* system_model = config->get_model();

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
}
