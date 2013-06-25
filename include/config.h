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

class parameter;

class configuration {
public:
    typedef std::pair<std::string, std::string> pair_type;

    static configuration* get_instance();

    bool enable_distinct_nbest() const;
    unsigned int get_pop_limit() const;
    unsigned int get_span_limit() const;
    unsigned int get_nbest_number() const;
    unsigned int get_thread_number() const;
    unsigned int get_feature_number() const;
    float get_weight(unsigned int index) const;
    float get_beam_threshold(const std::string& nonterminal) const;
    unsigned int get_beam_size(const std::string& nonterminal) const;
    void load_configuration_file(const char* filename);
    void get_parameter(std::vector<pair_type>& vec);
private:
    configuration();
    ~configuration();

    static configuration instance;

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
};

#endif /* __CONFIG_H__ */
