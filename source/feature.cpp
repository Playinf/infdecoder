/* feature.cpp */
#include <lm.h>
#include <rule.h>
#include <feature.h>
#include <hypothesis.h>

double previous_hypotheses_score_sum(const hypothesis* hypo, unsigned int id)
{
    auto prev_hypo = hypo->get_previous_hypothese();
    double score = 0.0;
    unsigned int size;

    if (prev_hypo == nullptr)
        return score;

    size = prev_hypo->size();

    for (unsigned int i = 0; i < size; i++) {
        hypothesis* h = prev_hypo->at(i);
        const feature* f = h->get_feature(id);

        score += f->get_score();
    }

    return score;
}

double translation_feature_function(const hypothesis* hypo, unsigned int id)
{
    const rule* r = hypo->get_rule();
    const feature* rule_feature = r->get_feature(id);
    double score = rule_feature->get_score();
    double prev_score = previous_hypotheses_score_sum(hypo, id);

    return score + prev_score;
}

double lm_feature_function(const hypothesis* hypo, unsigned int id)
{
    const rule* r = hypo->get_rule();
    auto& rule_body = r->get_target_rule_body();
    unsigned int rule_body_size = rule_body.size();
    unsigned int lm_order = lm::get_language_model_order();
    unsigned int indx = 0;
    std::vector<const std::string*> ngram;
    double score = 0.0;
    double prev_score = 0.0;
    double prefix_score = 0.0;
    bool calc = false;
    bool prefix = true;
    double full = 0.0;

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
                    //score = prev_fea->get_score();
                    prefix_score = prev_hypo->get_heuristic_score();
                } else {
                    double ngram_score = 0.0;
                    double full_score = 0.0;
                    lm::ngram_probability(ngram, full_score, ngram_score);

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

    double full_score = 0.0;
    double ngram_score = 0.0;
    lm::ngram_probability(ngram, full_score, ngram_score);

    if (prefix)
        prefix_score = full_score - ngram_score;
    score += ngram_score;
    full += full_score;

    score += prev_score;
    hypothesis* h = const_cast<hypothesis*>(hypo);
    h->set_heuristic_score(prefix_score);

    return score;
}

double word_penalty_feature_function(const hypothesis* hypo, unsigned int id)
{
    // log10(exp(-1))
    const double penalty = -0.4342944819032518276511289189166;
    unsigned int terminal_num = hypo->get_terminal_number();

    return terminal_num * penalty;
}

double extracted_rule_feature_function(const hypothesis* hypo, unsigned int id)
{
    const rule* r = hypo->get_rule();
    // log10(exp(-1))
    const double penalty = -0.4342944819032518276511289189166;
    const symbol* sym = r->get_start_symbol();
    double score = penalty;
    double prev_score = previous_hypotheses_score_sum(hypo, id);

    if (*sym->get_name() == "X")
        return score + prev_score;
    return prev_score;
}

double glue_rule_feature_function(const hypothesis* hypo, unsigned int id)
{
    const rule* r = hypo->get_rule();
    // log10(exp(-1))
    const double penalty = -0.4342944819032518276511289189166;
    const symbol* sym = r->get_start_symbol();
    double score = penalty;
    double prev_score = previous_hypotheses_score_sum(hypo, id);

    if (*sym->get_name() == "S")
        return score + prev_score;
    return prev_score;
}
