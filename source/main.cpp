/* main.cpp */
#include <ctime>
#include <string>
#include <cstdlib>
#include <fstream>
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
 * 2. add multi-thread support
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
        std::cout << val.first << ":" << val.second << std::endl;
    }
}

void output_hypothesis(const hypothesis* hypo, std::string& out)
{
    const rule* target_rule = hypo->get_rule();
    auto& rule_body = target_rule->get_target_rule_body();
    unsigned int size = rule_body.size();
    unsigned int nonterminal_index = 0;

    for (unsigned int i = 0; i < size; ++i) {
        const symbol* sym = rule_body[i];

        if (sym->is_terminal()) {
            const std::string& str = *sym->get_name();
            out.append(str);
            out.append(" ");
        } else {
            hypothesis *h = hypo->get_previous_hypothesis(nonterminal_index);
            ++nonterminal_index;
            output_hypothesis(h, out);
        }
    }
}

void output_nbest(const trellis_path* path, std::string& out)
{
    std::vector<const std::string*> output_vec;

    path->output(&output_vec);

    for (unsigned int i = 1; i < output_vec.size() - 1; i++) {
        const std::string& str = *output_vec[i];
        out.append(str);
        out.append(" ");
    }

    auto score_vec = path->get_score_vector();
    out.append(" ||| ");

    for (unsigned int i = 0; i < score_vec->size(); i++) {
        if (i == 0)
            out.append("lm: ");
        else if (i == 7)
            out.append("w: ");
        else if (i == 1)
            out.append("tm: ");
        out.append(std::to_string(score_vec->at(i)));
        out.append(" ");
    }

    out.append(" ||| ");
    out.append(std::to_string(path->get_total_score()));
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

int main(int argc, char** argv)
{
    configuration* config = configuration::get_instance();
    model* system_model = config->get_model();

    load_moses_options(argc, argv);
    config->load_parameter();
    config->load_weight();
    print_parameter();
    load_moses_model();

    translate();
}
