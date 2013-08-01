/* rule_heuristic.cpp */
#include <cmath>
#include <vector>
#include <rule.h>
#include <model.h>
#include <config.h>
#include <symbol.h>
#include <rule_tree.h>
#include <language_model.h>
#include <translation_model.h>

inline float untransform_lm_score(float score)
{
    return score / 2.302585093f;
}

void shift(std::vector<const std::string*>& ngram)
{
    unsigned int size = ngram.size();

    for (unsigned int i = 0; i < size; i++)
        ngram[i] = ngram[i + 1];
}

float moses_rule_heuristic_function(const rule* r)
{
    configuration* config = configuration::get_instance();
    model* system_model = config->get_model();
    auto& target_rule_body = r->get_target_rule_body();
    unsigned int size = target_rule_body.size();
    const float penalty = -0.434294482f;
    unsigned int tm_id = r->get_id();
    language_model* lm = system_model->get_language_model(0);
    translation_model* tm = system_model->get_translation_model(tm_id);
    float lm_weight = system_model->get_weight(lm->get_feature_id());
    unsigned int id = system_model->get_feature_id("word penalty");
    float terminal_num_weight = system_model->get_weight(id);
    unsigned target_terminal_number = r->get_terminal_number();
    unsigned int lm_order = lm->get_order();
    unsigned int ngram_len = 0;
    std::vector<const std::string*> ngram;
    float score = 0.0f;
    float full_score = 0.0f;

    for (unsigned int i = 0; i < size; i++) {
        const symbol* sym = target_rule_body[i];

        if (sym->is_nonterminal()) {
            ngram_len = 0;
            ngram.clear();
        } else {
            const std::string* str = sym->get_name();
            if (ngram_len < lm_order) {
                ngram_len++;
                ngram.push_back(str);
            } else {
                shift(ngram);
                ngram[lm_order - 1] = str;
            }
            float f = 0.0f;
            f = lm->word_probability(ngram);
            full_score += f;
        }
    }

    score += lm_weight * full_score;

    size = tm->get_feature_number();
    float other = 0.0f;

    for (unsigned int i = 0; i < size; i++) {
        unsigned int id = tm->get_feature_id(i);
        float weight = system_model->get_weight(id);

        other += weight * r->get_score(i);
    }

    float wp_score = terminal_num_weight * penalty * target_terminal_number;
    score += wp_score + other;

    return score;
}
