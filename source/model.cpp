/* model.cpp */
#include <utility>
#include <exception>
#include <model.h>
#include <rule_tree.h>
#include <language_model.h>
#include <translation_model.h>

model::model()
{
    feature_number = 0;
}

model::~model()
{
    unsigned int lm_num = language_model_vector.size();
    unsigned int tm_num = translation_model_vector.size();

    /* release memory */

    for (unsigned int i = 0; i < lm_num; i++) {
        language_model* lm = language_model_vector[i];
        delete lm;
    }

    for (unsigned int i = 0; i < tm_num; i++) {
        translation_model* tm = translation_model_vector[i];
        delete tm;
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

unsigned int model::get_feature_id(const std::string& name) const
{
    auto iter = feature_id_map.find(name);
    const unsigned int not_found = static_cast<unsigned int>(-1);

    if (iter == feature_id_map.end())
        return not_found;

    return iter->second;
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

model::size_type model::get_language_model_number() const
{
    return language_model_vector.size();
}

model::size_type model::get_translation_model_number() const
{
    return translation_model_vector.size();
}

language_model* model::get_language_model(size_type index) const
{
    return language_model_vector[index];
}

translation_model* model::get_translation_model(size_type index) const
{
    return translation_model_vector[index];
}

language_model* model::find_language_model(unsigned int id) const
{
    auto iter = feature_lm_map.find(id);

    if (iter == feature_lm_map.end())
        return nullptr;

    return iter->second;
}

translation_model* model::find_translation_model(unsigned int id) const
{
    auto iter = feature_tm_map.find(id);

    if (iter == feature_tm_map.end())
        return nullptr;

    return iter->second;
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

void model::add_language_model(language_model* lm)
{
    language_model_vector.push_back(lm);
}

void model::add_translation_model(translation_model* tm)
{
    translation_model_vector.push_back(tm);
}

void model::set_feature_source(unsigned int id, language_model* lm)
{
    feature_lm_map[id] = lm;
}

void model::set_feature_source(unsigned int id, translation_model* tm)
{
    feature_tm_map[id] = tm;
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
    feature_id_map[name] = feature_number;

    return feature_number++;
}
