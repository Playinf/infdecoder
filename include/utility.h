/*
 * utility.h
 * utility functions
 *
 * author: Playinf
 * playinf@stu.xmu.edu.cn
 *
 */
#ifndef __UTILITY_H__
#define __UTILITY_H__

#include <string>
#include <vector>

size_t data_hash(const char* data, size_t len);
size_t string_hash(const char* str);
void string_split(const std::string& s, const std::string& sep,
    std::vector<std::string>& vec);
int string_vector_compare(const std::vector<const std::string*>* s1,
    const std::vector<const std::string*>* s2);
void string_trim(const std::string& str, std::string& ret);

#endif /* __UTILITY_H__ */
