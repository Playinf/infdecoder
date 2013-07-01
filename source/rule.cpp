/* rule.h */
#include <rule.h>

std::vector<double> rule::lambda;

void rule::evaluate_score()
{
    unsigned int size = target_rule_body.size();
    //const double penalty = -0.4342944819032518276511289189166;
    const double penalty = -1.0f;
    double terminal_num_weight = lambda[5];
    double lm_weight = lambda[0];

    for (unsigned int i = 0; i < size; i++) {
        const symbol* sym = target_rule_body[i];

        score += lm::word_probability(*sym->get_name());
    }

    score *= lm_weight;
    size = feature_set.size();

    for (unsigned int i = 0; i < size; i++) {
        const feature& fea = feature_set[i];
        double weight = lambda[i + 1];

        score += weight * fea.get_score();
    }

    score += terminal_num_weight * penalty * target_terminal_num;
}
