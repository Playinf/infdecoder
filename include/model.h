/*
 * model.h
 *
 *
 * author: Playinf
 * email: playinf@stu.xmu.edu.cn
 *
 */
#ifndef __MODEL_H__
#define __MODEL_H__

#include <map>
#include <string>
#include <vector>
#include <feature_function.h>

class rule_tree;
class language_model;

class model {
public:
    typedef unsigned int size_type;
    model();
    ~model();

    size_type get_feature_number() const;
    float get_weight(unsigned int id) const;
    unsigned int get_feature_id(const std::string& name);
    unsigned int get_score_index(unsigned int tid, unsigned int fid) const;
    language_model* get_language_model_source(unsigned int id) const;
    const std::string& get_feature_name(unsigned int id) const;
    feature_function get_feature_function(unsigned int id) const;
    const std::string& get_feature_description(unsigned int id) const;

    size_type get_rule_table_number() const;
    size_type get_language_model_number() const;
    rule_tree* get_rule_table(size_type index) const;
    language_model* get_language_model(size_type index) const;

    void push_weight(float weight);
    void push_weight(std::vector<float>& weight);
    void add_rule_table(rule_tree* table);
    void add_language_model(language_model* lm);
    void set_score_index(unsigned int tid, unsigned int fid, unsigned int idx);
    void set_feature_source(unsigned int id, const language_model* lm);
    void set_feature_function(unsigned int id, feature_function func);
    unsigned int add_feature(const std::string& name, const std::string& des);
private:
    std::vector<float> lambda;
    std::vector<rule_tree*> rule_table_vector;
    std::vector<language_model*> language_model_vector;
    std::vector<std::string> feature_name_map;
    std::vector<unsigned int> feature_source_map;
    std::vector<unsigned int> feature_source_id_map;
    std::vector<std::string> feature_description_map;
    std::map<std::string, unsigned int> feature_id_map;
    std::vector<feature_function> feature_function_vector;
    unsigned int feature_number;
};

#endif /* __MODEL_H__ */
