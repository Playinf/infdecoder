/* parameter.cpp */
#include <cctype>
#include <string>
#include <vector>
#include <fstream>
#include <utility.h>
#include <parameter.h>

#include <iostream>
#include <typeinfo>

/* beam search */
static const float DEFAULT_BEAM_THRESHOLD = -10000.0;
static const unsigned int DEFAULT_BEAM_SIZE = 100;
/* cube pruning */
static const unsigned int DEFAULT_POP_LIMIT = 1000;
/* parsing */
static const unsigned int DEFAULT_SPAN_LIMIT = 20;
/* model */
static const unsigned int DEFAULT_FEATURE_NUMBER = 8;
/* decoder */
static const unsigned int DEFAULT_DISTINCT_NBEST = 0;
static const unsigned int DEFAULT_NBEST_NUMBER = 1;
static const unsigned int DEFAULT_THREAD_NUMBER = 1;

/*
enum token_type {
    LEFT_BRACKET, RIGHT_BRACKET, LEFT_BRACE, RIGHT_BRACE,
    QUOTE, COMMA, SEMICOLON, PERIOD,
    STRING, INTEGER, REAL, ARRAY
};
*/

enum parameter_type {
    STRING, INTEGER, REAL, ARRAY
};

static parameter_type parse_parameter_type(const std::string& value)
{
    char first = value[0];
    char last = value[value.size() - 1];

    if (first == '\"' && last == '\"')
        return STRING;

    if (first == '{' && last == '}')
        return ARRAY;

    if (value.find('.') != value.npos)
        return REAL;

    return INTEGER;
}

static void parse_parameter(const std::string& line, std::string& param,
    std::string& value)
{
    std::string::size_type pos1 = line.find('=');
    std::string::size_type pos2 = line.find(';');
    std::string param_name = line.substr(0, pos1);
    std::string param_value = line.substr(pos1 + 1, pos2 - pos1 - 1);

    string_trim(param_name, param);
    string_trim(param_value, value);
}

parameter::parameter()
{
    load_default();
}

parameter::parameter(const char* filename)
{
    load_default();
    load(filename);
}

parameter::~parameter()
{
}

void parameter::load_default()
{
    integer_parameter_map["beam_size"] = DEFAULT_BEAM_SIZE;
    integer_parameter_map["pop_limit"] = DEFAULT_POP_LIMIT;
    integer_parameter_map["span_limit"] = DEFAULT_SPAN_LIMIT;
    integer_parameter_map["distinct_nbest"] = DEFAULT_DISTINCT_NBEST;
    integer_parameter_map["nbest_number"] = DEFAULT_NBEST_NUMBER;
    integer_parameter_map["thread_number"] = DEFAULT_THREAD_NUMBER;
    integer_parameter_map["feature_number"] = DEFAULT_FEATURE_NUMBER;

    real_parameter_map["beam_threshold"] = DEFAULT_BEAM_THRESHOLD;
}

void parameter::insert_array_parameter(const std::string& name,
    const std::string& value)
{
    unsigned int count = 0;
    std::string::size_type spos = 1;
    std::string::size_type epos;
    std::string::size_type size = value.size();
    float val;

    while (spos < size) {
        epos = value.find(',', spos);

        if (epos == std::string::npos)
            epos = size - 1;

        val = std::stof(value.substr(spos, epos - spos));
        std::string param_name;
        param_name = name + "_" + std::to_string(count++);
        real_parameter_map[param_name] = val;
        spos = epos + 1;
    }
}

void parameter::load(const char* filename)
{
    std::ifstream input(filename);
    std::string line;

    while (std::getline(input, line)) {
        if (line[0] == '#' || line.empty())
            continue;

        std::string pname;
        std::string pvalue;
        parse_parameter(line, pname, pvalue);
        auto type = parse_parameter_type(pvalue);
        std::string::size_type start;
        std::string::size_type end;
        std::string::size_type len;

        switch (type) {
        case STRING:
            start = 1;
            end = pvalue.size() - 2;
            len = end - start + 1;
            string_parameter_map[pname] = pvalue.substr(start, len);
            break;
        case INTEGER:
            integer_parameter_map[pname] = std::stoi(pvalue);
            break;
        case REAL:
            real_parameter_map[pname] = std::stof(pvalue);
            break;
        case ARRAY:
            insert_array_parameter(pname, pvalue);
            break;
        default:
            break;
        }
    }
}

float parameter::get_real_parameter(const std::string& param)
{
    return real_parameter_map[param];
}

unsigned int parameter::get_integer_parameter(const std::string& param)
{
    return integer_parameter_map[param];
}

const std::string& parameter::get_string_parameter(const std::string& param)
{
    return string_parameter_map[param];
}

void parameter::get_parameter(std::vector<pair_type>& vec)
{
    for (auto& value_pair : string_parameter_map) {
        std::string name = value_pair.first;
        std::string value = value_pair.second;
        vec.push_back(std::make_pair(name, value));
    }

    for (auto& value_pair : integer_parameter_map) {
        std::string name = value_pair.first;
        std::string value = std::to_string(value_pair.second);
        vec.push_back(std::make_pair(name, value));
    }

    for (auto& value_pair : real_parameter_map) {
        std::string name = value_pair.first;
        std::string value = std::to_string(value_pair.second);
        vec.push_back(std::make_pair(name, value));
    }
}