/* language_model.cpp */
#include <language_model.h>

language_model::language_model()
{
    id = 0;
    feature_id = 0;
}

language_model::~language_model()
{
    // do nothing
}

unsigned int language_model::get_id() const
{
    return id;
}

unsigned int language_model::get_feature_id() const
{
    return feature_id;
}

language_model_type language_model::get_type() const
{
    return type;
}

void language_model::set_id(unsigned int id)
{
    this->id = id;
}

void language_model::set_type(language_model_type type)
{
    this->type = type;
}

void language_model::set_feature(unsigned int id)
{
    feature_id = id;
}
