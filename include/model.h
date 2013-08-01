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
class translation_model;

class model {
public:
    typedef unsigned int size_type;
    model();
    ~model();

    size_type get_feature_number() const;
    float get_weight(unsigned int id) const;
    unsigned int get_feature_id(const std::string& name) const;
    const std::string& get_feature_name(unsigned int id) const;
    feature_function get_feature_function(unsigned int id) const;
    const std::string& get_feature_description(unsigned int id) const;

    size_type get_language_model_number() const;
    size_type get_translation_model_number() const;
    language_model* get_language_model(size_type index) const;
    translation_model* get_translation_model(size_type index) const;
    language_model* find_language_model(unsigned int id) const;
    translation_model* find_translation_model(unsigned int id) const;

    void push_weight(float weight);
    void push_weight(std::vector<float>& weight);
    void add_language_model(language_model* lm);
    void add_translation_model(translation_model* lm);
    void set_feature_source(unsigned int id, language_model* lm);
    void set_feature_source(unsigned int id, translation_model* tm);
    void set_feature_function(unsigned int id, feature_function func);
    unsigned int add_feature(const std::string& name, const std::string& des);
private:
    std::vector<float> lambda;
    std::vector<std::string> feature_name_map;
    std::vector<std::string> feature_description_map;
    std::map<std::string, unsigned int> feature_id_map;
    std::vector<feature_function> feature_function_vector;
    std::vector<language_model*> language_model_vector;
    std::vector<translation_model*> translation_model_vector;
    std::map<unsigned int, language_model*> feature_lm_map;
    std::map<unsigned int, translation_model*> feature_tm_map;
    unsigned int feature_number;
};

#endif /* __MODEL_H__ */
