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

class parameter {
public:
    parameter();
    parameter(const char* filename);
    ~parameter();

    void load(const char* filename);

    const std::string& get_parameter(std::string& param) const;
private:
    void load_default();
    std::map<std::string, double> double_parameter_map;
    std::map<std::string, unsigned int> uint_parameter_map;
    std::map<std::string, std::string> string_parameter_map;
};

#endif /* __PARAMETER_H__ */
