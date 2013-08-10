/* feature.cpp */
#include <model.h>
#include <state.h>
#include <config.h>
#include <feature.h>
#include <feature_function.h>

feature::feature(unsigned int id)
{
    configuration* config = configuration::get_instance();
    model* system_model = config->get_model();

    score = 0.0f;
    feature_id = id;
    feature_state = nullptr;
    weight = system_model->get_weight(id);
    function = system_model->get_feature_function(feature_id);
}

feature::~feature()
{
    delete feature_state;
}

float feature::get_score() const
{
    return score;
}

float feature::get_weight() const
{
    return weight;
}

state* feature::get_state() const
{
    return feature_state;
}

unsigned int feature::get_id() const
{
    return feature_id;
}

void feature::add_state(state* s)
{
    feature_state = s;
}

void feature::evaluate(const hypothesis* hypo)
{
    score = function(hypo, feature_id);
}
