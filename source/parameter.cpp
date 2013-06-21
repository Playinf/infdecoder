/* parameter.cpp */
#include <parameter.h>

/* beam search */
static const double DEFAULT_BEAM_THRESHOLD = -10000.0;
static const unsigned int DEFAULT_BEAM_SIZE = 100;
/* cube pruning */
static const unsigned int DEFAULT_POP_LIMIT = 1000;
/* parsing */
static const unsigned int DEFAULT_SPAN_LIMIT = 20;
/* model */
static const unsigned int DEFAULT_FEATURE_NUMBER = 8;
/* decoder */
static const unsigned int DEFAULT_NBEST_NUMBER = 1;
static const unsigned int DEFAULT_THREAD_NUMBER = 1;

static void parse_parameter(std::string& line, std::string& param,
    std::string& value)
{
    size_t pos1 = line.find('=');
    size_t pos2 = line.find(';');
    param = line.substr(0, pos1 - 1);
    value = line.substr(pos1 + 1, pos2 - 1);

    param = string_trim(param);
    value = string_trim(value);
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
    uint_parameter_map["beam_size"] = DEFAULT_BEAM_SIZE;
    uint_parameter_map["pop_limit"] = DEFAULT_POP_LIMIT;
    uint_parameter_map["span_limit"] = DEFAULT_SPAN_LIMIT;
    uint_parameter_map["nbest_number"] = DEFAULT_NBEST_NUMBER;
    uint_parameter_map["thread_number"] = DEFAULT_THREAD_NUMBER;
    uint_parameter_map["feature_number"] = DEFAULT_FEATURE_NUMBER;

    double_parameter_map["beam_threshold"] = DEFAULT_BEAM_THRESHOLD;
}

void parameter::load(const char* filename)
{

}
