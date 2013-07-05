/* hypothesis.cpp */
#include <rule.h>
#include <model.h>
#include <config.h>
#include <symbol.h>
#include <utility.h>
#include <hypothesis.h>

hypothesis::hypothesis(const rule* r)
{
    configuration* config = configuration::get_instance();
    model* system_model = config->get_model();
    unsigned int feature_number = system_model->get_feature_number();

    target_rule = r;
    terminal_number = r->get_terminal_number();
    hypothesis_vector = nullptr;
    recombined_hypothesis = nullptr;
    log_linear_model.reserve(feature_number);

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

float hypothesis::get_score() const
{
    return score;
}

float hypothesis::get_heuristic_score() const
{
    return heuristic_score;
}

float hypothesis::get_total_score() const
{
    return score + heuristic_score;
}

const rule* hypothesis::get_rule() const
{
    return target_rule;
}

unsigned int hypothesis::get_terminal_number() const
{
    return terminal_number;
}

unsigned int hypothesis::get_feature_number() const
{
    return log_linear_model.size();
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

/* TODO: need to fix */
hypothesis* hypothesis::get_previous_hypothesis(unsigned int index) const
{
    if (hypothesis_vector == nullptr)
        return nullptr;

    return hypothesis_vector->at(index);
}

const std::vector<const std::string*>* hypothesis::get_prefix() const
{
    return &prefix;
}

const std::vector<const std::string*>* hypothesis::get_suffix() const
{
    return &suffix;
}

void hypothesis::evaluate_score()
{
    unsigned int size = log_linear_model.size();

    for (unsigned int i = 0; i < size; ++i) {
        feature fea = log_linear_model[i];
        fea.evaluate(this);
    }

    //score = log_linear_model.get_score();
}

void hypothesis::set_heuristic_score(float score)
{
    heuristic_score = score;
}

void hypothesis::recombine(hypothesis* hypo)
{
    if (recombined_hypothesis == nullptr){
        recombined_hypothesis = hypo;
    } else {
        hypothesis* p = this;
        hypothesis* recombined = p->recombined_hypothesis;
        double total_score = hypo->get_total_score();

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

void hypothesis::push_hypothesis(hypothesis* h)
{
    if (hypothesis_vector == nullptr)
        hypothesis_vector = new std::vector<hypothesis*>;

    hypothesis_vector->push_back(h);
    terminal_number += h->get_terminal_number();
}

void hypothesis::output(std::vector<const std::string*>& s) const
{
    auto& rule_body = target_rule->get_target_rule_body();
    unsigned int size = rule_body.size();
    unsigned int nonterminal_index = 0;

    for (unsigned int i = 0; i < size; ++i) {
        const symbol* sym = rule_body[i];

        if (sym->is_terminal())
            s.push_back(sym->get_name());
        else {
            hypothesis *hypo = get_previous_hypothesis(nonterminal_index);
            ++nonterminal_index;
            hypo->output(s);
        }
    }
}

int hypothesis::compare(const hypothesis* hypo) const
{
    int result;

    result = string_vector_compare(&prefix, hypo->get_prefix());

    if (result != 0)
        return result;

    result = string_vector_compare(&suffix, hypo->get_suffix());

    return result;
}

void hypothesis::calculate_prefix_suffix(unsigned int order)
{
    auto& rule_body = target_rule->get_target_rule_body();
    unsigned int rule_body_size = rule_body.size();
    std::vector<const std::string*> ngram;
    unsigned int nonterm_indx = 0;
    unsigned int lm_order = order;

    ngram.reserve(2 * terminal_number);

    /* calculate prefix and suffix */
    for (unsigned int i = 0; i < rule_body_size; ++i) {
        const symbol* sym = rule_body[i];

        if (sym->is_terminal())
            ngram.push_back(sym->get_name());
        else {
            hypothesis* hypo = get_previous_hypothesis(nonterm_indx++);
            auto hypo_prefix = hypo->get_prefix();
            auto hypo_suffix = hypo->get_suffix();
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

void hypothesis::calculate_prefix(std::vector<const std::string*>* out,
        size_type& size)
{
    auto& rule_body = target_rule->get_target_rule_body();
    size_type len = rule_body.size();
    size_type nonterminal_index = 0;

    for (size_type i = 0; i < len; ++i) {
        const symbol* sym = rule_body[i];

        if (sym->is_nonterminal()) {
            hypothesis* hypo = get_previous_hypothesis(nonterminal_index);
            hypo->calculate_prefix(out, size);
            ++nonterminal_index;
        } else {
            out->push_back(sym->get_name());
            --size;
        }

        if (size == 0)
            break;
    }
}

void hypothesis::calculate_suffix(std::vector<const std::string*>* out,
        size_type& size)
{
    auto& rule_body = target_rule->get_target_rule_body();
    size_type len = rule_body.size();

    if (prefix.size() == terminal_number) {
        size_type count = (terminal_number) < size ? terminal_number: size;
        size_type pos = terminal_number - 1;

        for (size_type i = 0; i < count; ++i) {
            const std::string* str = prefix[pos];
            out->push_back(str);
            --pos;
        }

        size -= count;
    } else {
        unsigned int nonterminal_num = len - target_rule->get_terminal_number();
        unsigned int nonterminal_index = nonterminal_num - 1;
        for (size_type i = len - 1; i >= 0; --i) {
            const symbol* sym = rule_body[i];

            if (sym->is_nonterminal()) {
                hypothesis* hypo;
                hypo = get_previous_hypothesis(nonterminal_index);
                hypo->calculate_suffix(out, size);
                --nonterminal_index;
            } else {
                out->push_back(sym->get_name());
                --size;
            }

            if (size == 0)
                break;
        }
    }
}
