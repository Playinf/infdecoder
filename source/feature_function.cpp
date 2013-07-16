/* feature_function.cpp */
#include <rule.h>
#include <model.h>
#include <config.h>
#include <symbol.h>
#include <feature.h>
#include <rule_tree.h>
#include <hypothesis.h>
#include <language_model.h>

float language_model_feature_function(const hypothesis* hypo, unsigned int id)
{
    float score = 0.0;
    float prev_score = 0.0;
    float prefix_score = 0.0;
    bool calc = false;
    bool prefix = true;
    float full = 0.0;
    const rule* r = hypo->get_rule();
    auto& rule_body = r->get_target_rule_body();
    unsigned int rule_body_size = rule_body.size();
    configuration* config = configuration::get_instance();
    model* system_model = config->get_model();
    language_model* lm = system_model->get_language_model_source(id);
    float weight = system_model->get_weight(id);
    unsigned int lm_order = lm->get_order();
    unsigned int indx = 0;
    std::vector<const std::string*> ngram;

    /* this test should be false */
    if (lm == nullptr)
        return 0.0f;

    for (unsigned int i = 0; i < rule_body_size; i++) {
        const symbol* sym = rule_body[i];

        if (sym->is_terminal()) {
            ngram.push_back(sym->get_name());
        } else {
            hypothesis* prev_hypo = hypo->get_previous_hypothesis(indx++);
            unsigned int prev_term_num = prev_hypo->get_terminal_number();
            const feature* prev_fea = prev_hypo->get_feature(id);
            auto hypo_prefix = prev_hypo->get_prefix();
            auto hypo_suffix = prev_hypo->get_suffix();
            auto iter_begin = hypo_prefix->begin();
            auto iter_end = hypo_prefix->end();

            ngram.insert(ngram.end(), iter_begin, iter_end);

            if (prev_term_num >= lm_order - 1) {
                calc = true;
                prev_score += prev_fea->get_score();
            }

            if (calc) {
                iter_begin = hypo_suffix->begin();
                iter_end = hypo_suffix->end();

                if (i == 0 && prev_term_num >= lm_order - 1) {
                    prefix_score = prev_hypo->get_heuristic_score();
                    /* unweighted prefix score */
                    prefix_score /= weight;
                } else {
                    float ngram_score = 0.0;
                    float full_score = 0.0;
                    lm->ngram_probability(ngram, full_score, ngram_score);

                    if (prefix)
                        prefix_score = full_score - ngram_score;
                    full += full_score;
                    score += ngram_score;
                }

                ngram.clear();
                ngram.insert(ngram.end(), iter_begin, iter_end);
                calc = false;
                prefix = false;
            }
        }
    }

    float full_score = 0.0;
    float ngram_score = 0.0;
    lm->ngram_probability(ngram, full_score, ngram_score);

    if (prefix)
        prefix_score = full_score - ngram_score;
    score += ngram_score;
    full += full_score;

    score += prev_score;
    hypothesis* h = const_cast<hypothesis*>(hypo);
    h->set_heuristic_score(prefix_score * weight);
    h->calculate_prefix_suffix(lm_order);

    return score;
}

float translation_model_feature_function(const hypothesis* h, unsigned int id)
{
    float score = 0.0f;
    const rule* r = h->get_rule();
    unsigned int size = h->get_previous_hypothesis_number();
    configuration* config = configuration::get_instance();
    model* system_model = config->get_model();
    unsigned int tree_id = r->get_rule_tree_id();
    rule_tree* tree = system_model->get_rule_tree(tree_id);

    score = tree->get_score(r, id);

    if (!size)
        return score;

    /* sum score from previous hypotheses */
    for (unsigned int i = 0; i < size; i++) {
        hypothesis* hypo = h->get_previous_hypothesis(i);
        const feature* f = hypo->get_feature(id);

        score += f->get_score();
    }

    return score;
}

float word_penalty_feature_function(const hypothesis* hypo, unsigned int id)
{
    const float penalty = -1.0f;
    unsigned int terminal_num = hypo->get_terminal_number();

    return terminal_num * penalty;
}
