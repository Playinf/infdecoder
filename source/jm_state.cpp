/* jm_state.cpp */
#include <rule.h>
#include <symbol.h>
#include <jm_state.h>
#include <utility.h>
#include <hypothesis.h>

jm_state::jm_state(const hypothesis* hypo, unsigned int id)
{
    score = 0.0f;
    this->id = id;
    this->affiliation_id = 0;
    state_hypothesis = hypo;
}

jm_state::~jm_state()
{
    // do nothing
}

float jm_state::get_score() const
{
    return score;
}

const std::vector<unsigned int>* jm_state::get_affiliation() const
{
    return &affiliation;
}

const std::vector<const std::string*>* jm_state::get_prefix() const
{
    return &prefix;
}

const std::vector<const std::string*>* jm_state::get_suffix() const
{
    return &suffix;
}

const std::vector<const std::string*>* jm_state::get_translation() const
{
    return &translation;
}

void jm_state::set_score(float score)
{
    this->score = score;
}

void jm_state::set_affiliation(std::vector<unsigned int>& vec)
{
    affiliation = vec;
}

void jm_state::set_translation(std::vector<const std::string*>& vec)
{
    translation = vec;
}

int jm_state::compare(const state* jmstate) const
{
    const jm_state* s = static_cast<const jm_state*>(jmstate);

    return compare(s);
}

int jm_state::compare(const jm_state* state) const
{
    int result;

    if (affiliation_id != state->affiliation_id)
        return affiliation_id - state->affiliation_id;

    result = string_vector_compare(&prefix, state->get_prefix());

    if (result != 0)
        return result;

    result = string_vector_compare(&suffix, state->get_suffix());

    return result;
}

void jm_state::calculate_prefix_suffix(unsigned int order)
{
    unsigned int size = translation.size();

    if (size <= order - 1) {
        for (unsigned int i = 0; i < size; i++) {
            const std::string* ptr = translation[i];
            prefix.push_back(ptr);
            suffix.push_back(ptr);
        }
    } else {
        for (unsigned int i = 0; i < order; i++) {
            const std::string* ptr = translation[i];
            prefix.push_back(ptr);
        }

        for (unsigned int i = size - (order - 1); i < size; i++) {
            const std::string* ptr = translation[i];
            suffix.push_back(ptr);
        }
    }
}
