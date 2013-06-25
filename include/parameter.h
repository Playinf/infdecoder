/*
 * parameter.h
 *
 *
 * author: Playinf
 * email: playinf@stu.xmu.edu.cn
 *
 */
#ifndef __PARAMETER_H__
#define __PARAMETER_H__

#include <map>
#include <string>
#include <vector>
#include <utility>

class parameter {
public:
    typedef std::pair<std::string, std::string> pair_type;
    parameter();
    parameter(const char* filename);
    ~parameter();

    void load(const char* filename);
    float get_real_parameter(const std::string& param);
    unsigned int get_integer_parameter(const std::string& param);
    const std::string& get_string_parameter(const std::string& param);
    void get_parameter(std::vector<pair_type>& vec);
private:
    void load_default();
    void insert_array_parameter(const std::string& n, const std::string& v);
    std::map<std::string, float> real_parameter_map;
    std::map<std::string, unsigned int> integer_parameter_map;
    std::map<std::string, std::string> string_parameter_map;
};

#endif /* __PARAMETER_H__ */
