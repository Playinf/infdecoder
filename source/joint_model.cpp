/* joint_model.cpp */
#include <joint_model.h>

joint_model::joint_model()
{
    // source to target mode by default
    model_type = joint_model_type::source_to_target;
}

joint_model::~joint_model()
{
    // do nothing
}

joint_model_type joint_model::get_model_type() const
{
    return model_type;
}

void joint_model::set_model_type(joint_model_type t)
{
    model_type = t;
}
