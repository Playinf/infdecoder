/* config.cpp */
#include <string>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <config.h>
#include <utility.h>

std::map<std::string, std::string> parameter::param_map;

static void parse_parameter(std::string& line, std::string& name, 
    std::string& value)
{
    size_t pos1 = line.find("\"", 0);
    size_t pos2;
    
    if (pos1 == line.npos) {
        std::cout << "bad configuration file" << std::endl;
        std::exit(-1);
    }

    pos2 = line.find("\"", pos1 + 1);
    name = line.substr(pos1 + 1, pos2 - pos1 - 1);

    pos1 = line.find("\"", pos2 + 1);
    pos2 = line.find("\"", pos1 + 1);

    value = line.substr(pos1 + 1, pos2 - pos1 - 1);
}

void parameter::load_configuation_file(const char* path)
{
    std::ifstream config_file(path);
    std::string line;
    std::string name;
    std::string value;

    if (config_file.fail()) {
        std::cout << "cannot open file `" << path << "'" << std::endl;
        std::exit(-1);
    }

    while (std::getline(config_file, line)) {
        if (line.length() == 0 || line[0] == '%')
            continue;

        parse_parameter(line, name, value);
        param_map[name] = value;
    }

    config_file.close();
}

const std::string& parameter::get_parameter(const char* param)
{
    std::string temp(param);

    return param_map[temp];
}
