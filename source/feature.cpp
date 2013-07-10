/* feature.cpp */
#include <model.h>
#include <config.h>
#include <feature.h>
#include <feature_function.h>

feature::feature(unsigned int id)
{
    configuration* config = configuration::get_instance();
    model* system_model = config->get_model();

    score = 0.0f;
    feature_id = id;
    weight = system_model->get_weight(id);
    feature_func = system_model->get_feature_function(feature_id);
}

feature::~feature()
{
    /* do nothing */
}

float feature::get_score() const
{
    return score;
}

float feature::get_weight() const
{
    return weight;
}

unsigned int feature::get_id() const
{
    return feature_id;
}

void feature::evaluate(const hypothesis* hypo)
{
    score = feature_func(hypo, feature_id);
}
