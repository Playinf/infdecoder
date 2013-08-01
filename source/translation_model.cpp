/* translation_model.cpp */
#include <rule_tree.h>
#include <translation_model.h>

translation_model::translation_model()
{
    feature_number = 0;
    rule_table = nullptr;
}

translation_model::~translation_model()
{
    if (rule_table != nullptr)
        delete rule_table;
}

unsigned int translation_model::get_id() const
{
    return id;
}

rule_tree* translation_model::get_rule_tree() const
{
    return rule_table;
}

unsigned int translation_model::get_feature_number() const
{
    return feature_number;
}

unsigned int translation_model::get_index(unsigned int id) const
{
    const unsigned int not_found = static_cast<unsigned int>(-1);

    for (unsigned int i = 0; i < feature_number; i++) {
        if (feature_vector[i] == id)
            return i;
    }

    return not_found;
}

unsigned int translation_model::get_feature_id(unsigned int index) const
{
    return feature_vector[index];
}

void translation_model::set_id(unsigned int id)
{
    this->id = id;
}

void translation_model::add_feature(unsigned int id)
{
    feature_vector.push_back(id);
    feature_number++;
}

void translation_model::add_rule_tree(rule_tree* tree)
{
    rule_table = tree;
}
