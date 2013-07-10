/* model.cpp */
#include <utility>
#include <exception>
#include <model.h>
#include <rule_tree.h>
#include <language_model.h>

model::model()
{
    feature_number = 0;
}

model::~model()
{
    unsigned int table_num = rule_table_vector.size();
    unsigned int lm_num = language_model_vector.size();
    /* release memory */
    for (unsigned int i = 0; i < table_num; i++) {
        rule_tree* t = rule_table_vector[i];
        delete t;
    }

    for (unsigned int i = 0; i < lm_num; i++) {
        language_model* lm = language_model_vector[i];
        delete lm;
    }
}

model::size_type model::get_feature_number() const
{
    return feature_number;
}

float model::get_weight(unsigned int id) const
{
    return lambda[id];
}

unsigned int model::get_feature_id(const std::string& name)
{
    feature_id_map.find(name);
}

unsigned int model::get_score_index(unsigned int tid, unsigned int fid) const
{
    if (feature_source_id_map[fid] == tid)
        return feature_source_map[fid];

    return -1;
}

language_model* model::get_language_model_source(unsigned int id) const
{
    unsigned int index = feature_source_map[id];

    return language_model_vector[index];
}

const std::string& model::get_feature_name(unsigned int id) const
{
    return feature_name_map[id];
}

feature_function model::get_feature_function(unsigned int id) const
{
    return feature_function_vector[id];
}

const std::string& model::get_feature_description(unsigned int id) const
{
    return feature_description_map[id];
}

model::size_type model::get_rule_table_number() const
{
    return rule_table_vector.size();
}

model::size_type model::get_language_model_number() const
{
    return language_model_vector.size();
}

rule_tree* model::get_rule_table(size_type index) const
{
    return rule_table_vector[index];
}

language_model* model::get_language_model(size_type index) const
{
    return language_model_vector[index];
}

void model::push_weight(float weight)
{
    lambda.push_back(weight);
}

void model::push_weight(std::vector<float>& weight)
{
    unsigned int weight_num = weight.size();
    for (unsigned int i = 0; i < weight_num; i++)
        lambda.push_back(weight[i]);
}

void model::add_rule_table(rule_tree* table)
{
    rule_table_vector.push_back(table);
}

void model::add_language_model(language_model* lm)
{
    language_model_vector.push_back(lm);
}

void model::set_score_index(unsigned int tid, unsigned int fid,
    unsigned int index)
{
    unsigned int size = feature_source_map.size();

    if (fid == size) {
        feature_source_map.push_back(index);
        feature_source_id_map.push_back(tid);
    } else if (fid < size) {
        feature_source_map[fid] = index;
        feature_source_id_map[fid] = tid;
    } else
        throw std::exception();
}

void model::set_feature_source(unsigned int id, const language_model* lm)
{
    unsigned int index;
    unsigned int size = feature_source_map.size();
    unsigned int lm_num = language_model_vector.size();

    for (unsigned int i = 0; i < lm_num; i++) {
        language_model* lmodel = language_model_vector[i];
        if (lmodel == lm)
            index = i;
    }

    if (id == size) {
        feature_source_map.push_back(index);
        feature_source_id_map.push_back(index);
    } else if (id < size) {
        feature_source_map[id] = index;
        feature_source_id_map[id] = index;
    } else
        throw std::exception();
}

void model::set_feature_function(unsigned int id, feature_function func)
{
    unsigned int size = feature_function_vector.size();

    if (id == size)
        feature_function_vector.push_back(func);
    else if (id < size)
        feature_function_vector[id] = func;
    else
        throw std::exception();
}

unsigned int model::add_feature(const std::string& name, const std::string& des)
{
    auto result = feature_id_map.find(name);

    if (result != feature_id_map.end())
        return result->second;

    feature_name_map.push_back(name);
    feature_description_map.push_back(des);

    return feature_number++;
}
