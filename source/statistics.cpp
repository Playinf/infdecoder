/* statistics.cpp */
#include <statistics.h>

statistics* statistics::instance;

statistics::statistics()
{
    // do nothing
}

statistics::~statistics()
{
    // do nothing
}

void statistics::add_value(const std::string& name, unsigned int value)
{
    statistics_map[name] += value;
}

unsigned int statistics::get_value(const std::string& name)
{
    return statistics_map[name];
}

void statistics::get_statistics(vector_type* result)
{
    for (auto& val : statistics_map) {
        result->push_back(std::make_pair(val.first, val.second));
    }
}
