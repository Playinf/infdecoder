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

class configuration {
public:
    unsigned int get_pop_limit() const;
    unsigned int get_span_limit() const;
    unsigned int get_nbest_number() const;
    unsigned int get_thread_number() const;
    unsigned int get_feature_number() const;
    unsigned int get_weight(unsigned int index) const;
    double get_beam_threshold(const std::string& nonterminal) const;
    unsigned int get_beam_size(const std::string& nonterminal) const;
private:
    configuration();
    ~configuration();

    static configuration instance;


};

#endif /* __CONFIG_H__ */
