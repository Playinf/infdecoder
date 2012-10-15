/* decoder.cpp */
#include <ctime>
#include <string>
#include <vector>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <config.h>
#include <decoder.h>

decoder::decoder()
{
    tree_vector.reserve(2);
    syntax_analyzer = nullptr;
}

decoder::~decoder()
{
    unsigned int size = tree_vector.size();

    for (unsigned int i = 0; i < size; i++) {
        rule_tree* tree = tree_vector[i];
        delete tree;
    }

    if (syntax_analyzer != nullptr)
        delete syntax_analyzer;
}

void decoder::load_rule_table(io_function func)
{
    const std::string* rule_table_name;
    std::ifstream rule_table_file;
    rule_table_name = &parameter::get_parameter("rule-file");
    const char* file_name = rule_table_name->c_str();
    std::clock_t begin;
    std::clock_t end;
    std::clock_t elapsed_time;
    void **argv = new void*[2];

    rule_table_file.open(file_name);

    if (rule_table_file.fail()) {
        std::cout << "cannot open file `" << file_name << "'";
        std::cout << std::endl;
        std::exit(-1);
    }

    std::cout << "loading rules from " << file_name << std::endl;

    rule_tree* tree = new rule_tree;
    argv[0] = (void*) &rule_table_file;
    argv[1] = (void*) tree;

    begin = std::clock();
    func((void*) argv);
    end = std::clock();
    elapsed_time = (end - begin) / CLOCKS_PER_SEC;
    std::cout << "elapsed time: " << elapsed_time << "s" << std::endl;
    tree_vector.push_back(tree);
    rule_table_file.close();

    std::cout << "sort and pruning rule tree" << std::endl;
    begin = std::clock();
    tree->sort();
    end = std::clock();
    elapsed_time = (end - begin) / CLOCKS_PER_SEC;
    std::cout << "elapsed time: " << elapsed_time << "s" << std::endl;

    rule_table_name = &parameter::get_parameter("special-rule-file");
    file_name = rule_table_name->c_str();

    rule_table_file.open(file_name);

    if (rule_table_file.fail()) {
        std::cout << "cannot open file `" << file_name << "'";
        std::cout << std::endl;
        std::exit(-1);
    }

    std::cout << "loading special rules from " << file_name << std::endl;
    
    tree = new rule_tree;
    argv[0] = (void*) &rule_table_file;
    argv[1] = (void*) tree;
    
    begin = std::clock();
    func((void*) argv);
    end = std::clock();
    elapsed_time = (end - begin) / CLOCKS_PER_SEC;
    std::cout << "elapsed time: " << elapsed_time << "s" << std::endl;
    tree_vector.push_back(tree);
    rule_table_file.close();

    delete[] argv;
}

void decoder::translate(io_function func)
{
    std::ifstream input_file;
    std::ofstream output_file;
    const std::string* filename;
    std::clock_t begin;
    std::clock_t end;
    std::clock_t elapsed_time;
    
    syntax_analyzer = new parser(&tree_vector);
    
    filename = &parameter::get_parameter("input-file");
    input_file.open(*filename); 

    if (input_file.fail()) {
        std::cout << "cannot open input file `";
        std::cout << *filename << "'" << std::endl;
        std::exit(-1);
    }
    
    filename = &parameter::get_parameter("output-file");
    output_file.open(*filename);

    if (output_file.fail()) {
        std::cout << "cannot open ouput file `";
        std::cout << *filename << "'" << std::endl;
        std::exit(-1);
    }

    std::string nbest_str;
    nbest_str = parameter::get_parameter("nbest");
    nbest = std::atoi(nbest_str.c_str());

    void** argv = new void*[4];
    argv[0] = (void*) &input_file;
    argv[1] = (void*) &output_file;
    argv[2] = (void*) syntax_analyzer;
    argv[3] = (void*) &nbest;
    
    begin = std::clock();
    func((void*) argv);
    end = std::clock();
    elapsed_time = (end - begin) / CLOCKS_PER_SEC;
    std::cout << "elapsed time: " << elapsed_time << "s" << std::endl;

    delete[] argv;
    input_file.close();
    output_file.close();
}
