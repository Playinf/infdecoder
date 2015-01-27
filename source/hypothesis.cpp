/* hypothesis.cpp */
#include <rule.h>
#include <model.h>
#include <state.h>
#include <config.h>
#include <symbol.h>
#include <hypothesis.h>

hypothesis::hypothesis(const rule* r)
{
    configuration* config = configuration::get_instance();
    model* system_model = config->get_model();
    unsigned int feature_number = system_model->get_feature_number();
    unsigned int nonterm_number = r->get_nonterminal_number();

    score = 0.0f;
    target_rule = r;
    terminal_number = r->get_terminal_number();
    hypothesis_vector = nullptr;
    recombined_hypothesis = nullptr;
    log_linear_model.reserve(feature_number);

    if (nonterm_number) {
        hypothesis_vector = new std::vector<hypothesis*>;
        hypothesis_vector->resize(nonterm_number);
    }

    for (unsigned int i = 0; i < feature_number; i++)
        log_linear_model.push_back(feature(i));
}

hypothesis::~hypothesis()
{
    if (hypothesis_vector != nullptr)
        delete hypothesis_vector;

    /* recursively delete recombined hypothesis */
    if (recombined_hypothesis != nullptr) {
        hypothesis* h = recombined_hypothesis;
        delete h;
    }
}

const rule* hypothesis::get_rule() const
{
    return target_rule;
}

float hypothesis::get_total_score() const
{
    return score;
}

unsigned int hypothesis::get_terminal_number() const
{
    return terminal_number;
}

unsigned int hypothesis::get_feature_number() const
{
    return log_linear_model.size();
}

const partial_rule* hypothesis::get_partial_rule() const
{
    return source_rule;
}

const feature* hypothesis::get_feature(unsigned int index) const
{
    return &log_linear_model[index];
}

const symbol* hypothesis::get_start_symbol(unsigned int index) const
{
    return target_rule->get_start_symbol(index);
}

hypothesis* hypothesis::get_recombined_hypothesis() const
{
    return recombined_hypothesis;
}

hypothesis::size_type hypothesis::get_previous_hypothesis_number() const
{
    if (hypothesis_vector == nullptr)
        return 0;

    return hypothesis_vector->size();
}

std::vector<hypothesis*>* hypothesis::get_hypothesis_vector() const
{
    return hypothesis_vector;
}

hypothesis* hypothesis::get_previous_hypothesis(unsigned int index) const
{
    if (hypothesis_vector == nullptr)
        return nullptr;

    return hypothesis_vector->at(index);
}

void hypothesis::evaluate_score()
{
    unsigned int size = log_linear_model.size();

    for (unsigned int i = 0; i < size; ++i) {
        float weight;
        feature& fea = log_linear_model[i];
        weight = fea.get_weight();
        fea.evaluate(this);
        score += fea.get_score() * weight;
    }
}

void hypothesis::recombine(hypothesis* hypo)
{
    if (recombined_hypothesis == nullptr){
        recombined_hypothesis = hypo;
    } else {
        hypothesis* p = this;
        hypothesis* recombined = p->recombined_hypothesis;
        float total_score = hypo->get_total_score();

        while (recombined) {
            if (recombined->get_total_score() < total_score)
                break;
            p = recombined;
            recombined = p->recombined_hypothesis;
        }

        hypo->recombined_hypothesis = p->recombined_hypothesis;
        p->recombined_hypothesis = hypo;
    }
}

int hypothesis::compare(const hypothesis* hypo) const
{
    auto& feature_vector = hypo->log_linear_model;
    unsigned int size = log_linear_model.size();

    for (unsigned int i = 0; i < size; i++) {
        const feature& f1 = log_linear_model[i];
        const feature& f2 = feature_vector[i];
        state* s1 = f1.get_state();
        state* s2 = f2.get_state();
        int ret;

        if (s1 == nullptr || s2 == nullptr)
            continue;

        ret = s1->compare(s2);

        if (ret != 0)
            return ret;
    }

    return 0;
}

void hypothesis::set_partial_rule(const partial_rule *rule)
{
    source_rule = rule;
}

void hypothesis::set_previous_hypothesis(unsigned int index, hypothesis* h)
{
    auto& hypo = hypothesis_vector->at(index);
    hypo = h;
    terminal_number += h->terminal_number;
}
