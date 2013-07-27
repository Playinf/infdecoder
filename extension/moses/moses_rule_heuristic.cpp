/* rule_heuristic.cpp */
#include <cmath>
#include <rule.h>
#include <model.h>
#include <config.h>
#include <symbol.h>
#include <rule_tree.h>
#include <language_model.h>

float moses_rule_heuristic_function(const rule* r)
{
    configuration* config = configuration::get_instance();
    model* system_model = config->get_model();
    auto& target_rule_body = r->get_target_rule_body();
    unsigned int size = target_rule_body.size();
    const float penalty = -0.434294482f;
    rule_tree* tree = system_model->get_rule_tree(r->get_rule_tree_id());
    language_model* lm = system_model->get_language_model(0);
    float lm_weight = system_model->get_weight(lm->get_feature_id());
    unsigned int id = system_model->get_feature_id("word penalty");
    float terminal_num_weight = system_model->get_weight(id);
    unsigned target_terminal_number = r->get_terminal_number();
    std::vector<const std::string*> ngram;
    float score = 0.0f;
    float full_score = 0.0f;
    float ngram_score = 0.0f;

    for (unsigned int i = 0; i < size; i++) {
        const symbol* sym = target_rule_body[i];

        if (sym->is_nonterminal()) {
            float f = 0.0f;
            float n = 0.0f;
            lm->ngram_probability(ngram, f, n);
            full_score += f;
            ngram_score += n;
            ngram.clear();
        } else {
            ngram.push_back(sym->get_name());
        }
    }

    float f = 0.0f;
    float n = 0.0f;
    lm->ngram_probability(ngram, f, n);

    full_score += f;
    ngram_score += n;

    score += lm_weight * full_score;

    size = tree->get_feature_number();

    for (unsigned int i = 0; i < size; i++) {
        unsigned int id = tree->get_feature_id(i);
        float weight = system_model->get_weight(id);

        score += weight * r->get_score(i);
    }

    score += terminal_num_weight * penalty * target_terminal_number;

    return score;
}
