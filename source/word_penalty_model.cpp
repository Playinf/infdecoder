/* word_penalty_model.cpp */
#include <penalty_model.h>

word_penalty_model::word_penalty_model()
{
    penalty = 0.0f;
}

word_penalty_model::~word_penalty_model()
{
    // do nothing
}

float word_penalty_model::get_penalty() const
{
    return penalty;
}

unsigned int word_penalty_model::get_id() const
{
    return id;
}

void word_penalty_model::set_id(unsigned int id)
{
    this->id = id;
}

void word_penalty_model::set_penalty(float score)
{
    penalty = score;
}
