/*
 * statistics.h
 *
 *
 * author: Playinf
 * email: playinf@stu.xmu.edu.cn
 *
 */
#ifndef __STATISTICS_H__
#define __STATISTICS_H__

#include <string>
#include <vector>
#include <utility>
#include <unordered_map>

class statistics {
public:
    typedef std::vector<std::pair<std::string, unsigned int>> vector_type;
    ~statistics();

    static statistics* get_instance();
    void add_value(const std::string& name, unsigned int value);
    unsigned int get_value(const std::string& name);
    void get_statistics(vector_type* result);
private:
    statistics();

    static statistics* instance;
    std::unordered_map<std::string, unsigned int> statistics_map;
};

#endif /* __STATISTICS_H__ */
