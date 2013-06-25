/* test_config.cpp */
#include <fstream>
#include <iostream>
#include <config.h>

void print_parameters()
{
    configuration* system_configuration;
    std::vector<std::pair<std::string, std::string>> vec;

    system_configuration = configuration::get_instance();
    system_configuration->get_parameter(vec);

    for (unsigned int i = 0; i < vec.size(); i++) {
        auto& value = vec[i];
        std::cout << value.first << ": ";
        std::cout << value.second << std::endl;
    }
}

int main(int argc, char** argv)
{
    configuration* system_config;
    system_config = configuration::get_instance();

    system_config->load_configuration_file("configfile");
    print_parameters();
}
