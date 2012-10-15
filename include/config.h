/*
 * config.h
 * configuaration file
 *
 * author: Playinf
 * playinf@stu.xmu.edu.cn
 *
 */
#ifndef __CONFIG_H__
#define __CONFIG_H__

#include <map>

class parameter {
public:
    static void load_configuation_file(const char* path);
    static const std::string& get_parameter(const char* param);
private:
    static std::map<std::string, std::string> param_map;
};

#endif /* __CONFIG_H__ */
