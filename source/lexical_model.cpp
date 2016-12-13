/* lexical_model.cpp */
#include <lexical_model.h>

lexical_model::lexical_model()
{
    // source to target mode by default
    model_type = lexical_model_type::source_to_target;
}

lexical_model::~lexical_model()
{
    // do nothing
}

lexical_model_type lexical_model::get_model_type() const
{
    return model_type;
}

void lexical_model::set_model_type(lexical_model_type t)
{
    model_type = t;
}

