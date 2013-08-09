/* hypothesis_state.cpp */
#include <rule.h>
#include <symbol.h>
#include <utility.h>
#include <hypothesis.h>

hypothesis_state::hypothesis_state(const hypothesis* hypo, unsigned int id)
{
    this->id = id;
    heuristic_score = 0.0f;
    state_hypothesis = hypo;
}

hypothesis_state::~hypothesis_state()
{
    /* do nothing */
}

float hypothesis_state::get_heuristic_score() const
{
    return heuristic_score;
}

const std::vector<const std::string*>* hypothesis_state::get_prefix() const
{
    return &prefix;
}

const std::vector<const std::string*>* hypothesis_state::get_suffix() const
{
    return &suffix;
}

void hypothesis_state::set_heuristic_score(float score)
{
    heuristic_score = score;
}

int hypothesis_state::compare(const hypothesis_state* state) const
{
    int result;

    result = string_vector_compare(&prefix, state->get_prefix());

    if (result != 0)
        return result;

    result = string_vector_compare(&suffix, state->get_suffix());

    return result;
}

void hypothesis_state::calculate_prefix_suffix(unsigned int order)
{
    const hypothesis* h = state_hypothesis;
    const rule* target_rule = h->get_rule();
    auto& rule_body = target_rule->get_target_rule_body();
    unsigned int rule_body_size = rule_body.size();
    std::vector<const std::string*> ngram;
    unsigned int nonterm_indx = 0;
    unsigned int lm_order = order;
    unsigned int terminal_number = h->get_terminal_number();

    ngram.reserve(2 * terminal_number);

    /* calculate prefix and suffix */
    for (unsigned int i = 0; i < rule_body_size; ++i) {
        const symbol* sym = rule_body[i];

        if (sym->is_terminal())
            ngram.push_back(sym->get_name());
        else {
            hypothesis* hypo = h->get_previous_hypothesis(nonterm_indx++);
            auto hypo_prefix = hypo->get_prefix(id);
            auto hypo_suffix = hypo->get_suffix(id);
            unsigned int hypo_prefix_size = hypo_prefix->size();
            unsigned int hypo_suffix_size = hypo_suffix->size();
            unsigned int hypo_nonterm_num = hypo->get_terminal_number();

            for (unsigned int j = 0; j < hypo_prefix_size; ++j)
                ngram.push_back(hypo_prefix->at(j));

            if (hypo_nonterm_num < lm_order)
                continue;

            for (unsigned int j = 0; j < hypo_suffix_size; ++j)
                ngram.push_back(hypo_suffix->at(j));
        }
    }

    if (terminal_number < lm_order) {
        for (unsigned int i = 0; i < terminal_number; ++i) {
            const std::string* str = ngram[i];
            prefix.push_back(str);
            suffix.push_back(str);
        }
    } else {
        unsigned int ngram_size = ngram.size();

        for (unsigned int i = 0; i < lm_order - 1; ++i) {
            const std::string* str = ngram[i];
            prefix.push_back(str);
        }

        for (unsigned int i = 0; i < lm_order - 1; ++i) {
            unsigned int j = ngram_size - lm_order + i + 1;
            const std::string* str = ngram[j];
            suffix.push_back(str);
        }
    }
}
