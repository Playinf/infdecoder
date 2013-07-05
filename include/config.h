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

#include <vector>

class model;
class parameter;

class configuration {
public:
    typedef std::pair<std::string, std::string> pair_type;
    ~configuration();

    configuration(const configuration& conf) = delete;
    configuration& operator=(const configuration& conf) = delete;

    model* get_model() const;
    bool enable_distinct_nbest() const;
    unsigned int get_pop_limit() const;
    unsigned int get_span_limit() const;
    unsigned int get_nbest_number() const;
    unsigned int get_thread_number() const;
    unsigned int get_feature_number() const;
    float get_weight(unsigned int index) const;
    float get_beam_threshold(const std::string& nonterminal) const;
    unsigned int get_beam_size(const std::string& nonterminal) const;
    parameter* get_parameter() const;

    void load_parameter_file(const char* filename);

    static configuration* get_instance();
private:
    configuration();

    model* system_model;
    parameter* parameter_set;
    unsigned int beam_size;
    unsigned int pop_limit;
    unsigned int span_limit;
    unsigned int distinct_nbest;
    unsigned int nbest_number;
    unsigned int thread_number;
    unsigned int feature_number;
    float beam_threshold;
    std::vector<float> weight_vector;
    std::string rule_table_path;
    std::string special_rule_table_path;

    static configuration instance;
};

#endif /* __CONFIG_H__ */
