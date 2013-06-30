/* config.cpp */
#include <string>
#include <vector>
#include <fstream>
#include <iostream>
#include <config.h>
#include <utility.h>
#include <parameter.h>

/* instance variable */
configuration configuration::instance;

configuration::configuration()
{
    parameter_set = new parameter();
}

configuration::~configuration()
{
    delete parameter_set;
}

bool configuration::enable_distinct_nbest() const
{
    return distinct_nbest != 0;
}

unsigned int configuration::get_pop_limit() const
{
    return pop_limit;
}

unsigned int configuration::get_span_limit() const
{
    return span_limit;
}

unsigned int configuration::get_nbest_number() const
{
    return nbest_number;
}

unsigned int configuration::get_thread_number() const
{
    return thread_number;
}

unsigned int configuration::get_feature_number() const
{
    return feature_number;
}

float configuration::get_weight(unsigned int index) const
{
    return weight_vector[index];
}

float configuration::get_beam_threshold(const std::string& nonterminal) const
{
    std::string name = nonterminal + "_beam_threshold";
    float threshold = parameter_set->get_real_parameter(name);

    if (threshold != 0.0)
        return threshold;

    return beam_threshold;
}

unsigned int configuration::get_beam_size(const std::string& nonterminal) const
{
    std::string name = nonterminal + "_beam_size";
    int size = parameter_set->get_integer_parameter(name);

    if (size != 0)
        return size;

    return beam_size;
}

void configuration::load_configuration_file(const char* filename)
{
    parameter_set->load(filename);

    beam_size = parameter_set->get_integer_parameter("beam_size");
    pop_limit = parameter_set->get_integer_parameter("pop_limit");
    span_limit = parameter_set->get_integer_parameter("span_limit");
    distinct_nbest = parameter_set->get_integer_parameter("distinct_nbest");
    nbest_number = parameter_set->get_integer_parameter("nbest_number");
    thread_number = parameter_set->get_integer_parameter("thread_number");
    feature_number = parameter_set->get_integer_parameter("feature_number");
    beam_threshold = parameter_set->get_real_parameter("beam_threshold");

    for (unsigned int i = 0; i > feature_number; i++) {
        std::string name = "weight-" + std::to_string(i);
        float val = parameter_set->get_real_parameter(name);

        weight_vector.push_back(val);
    }
}

void configuration::get_parameter(std::vector<pair_type>& vec)
{
    return parameter_set->get_parameter(vec);
}

configuration* configuration::get_instance()
{
    return &instance;
}
