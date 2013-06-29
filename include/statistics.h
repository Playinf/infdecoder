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

#include <map>
#include <string>
#include <vector>
#include <utility>

class statistics {
public:
    typedef std::vector<std::pair<std::string, unsigned int>> vector_type;
    ~statistics();

    void add_value(const std::string& name, unsigned int value);
    unsigned int get_value(const std::string& name);
    void get_statistics(vector_type* result);
    static statistics* get_instance();
private:
    statistics();

    std::map<std::string, unsigned int> statistics_map;
    static statistics* instance;
};

#endif /* __STATISTICS_H__ */
