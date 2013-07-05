/* feature.cpp */
#include <model.h>
#include <config.h>
#include <feature.h>
#include <feature_function.h>

feature::feature(unsigned int id)
{
    score = 0.0f;
    feature_id = id;
}

feature::~feature()
{
    /* do nothing */
}

float feature::get_score() const
{
    return score;
}

unsigned int feature::get_id() const
{
    return feature_id;
}

void feature::evaluate(const hypothesis* hypo)
{
    feature_function func;
    configuration* config = configuration::get_instance();
    model* system_model = config->get_model();
    func = system_model->get_feature_function(feature_id);

    score = func(hypo, feature_id);
}
