/* hypothesis.cpp */
#include <lm.h>
#include <rule.h>
#include <model.h>
#include <config.h>
#include <utility.h>
#include <hypothesis.h>

#include <iostream>

hypothesis::hypothesis(rule* r)
{
    unsigned int lm_order = lm::get_language_model_order();

    target_rule = r;
    terminal_number = r->get_terminal_num();
    next_hypothesis = nullptr;
    hypothesis_vector = nullptr;
    recombined_hypothesis = nullptr;
    prefix.reserve(lm_order - 1);
    suffix.reserve(lm_order - 1);
}

hypothesis::~hypothesis()
{
    if (hypothesis_vector)
        delete hypothesis_vector;

    /* recursively delete recombined hypothesis */
    if (recombined_hypothesis) {
        hypothesis* h = recombined_hypothesis;
        delete h;
    }
}

const symbol* hypothesis::get_start_symbol() const
{
    return target_rule->get_start_symbol();
}

void hypothesis::push_hypothesis(hypothesis* h)
{
    if (hypothesis_vector == nullptr)
        hypothesis_vector = new std::vector<hypothesis*>;

    hypothesis_vector->push_back(h);
    terminal_number += h->get_terminal_number();
}

void hypothesis::evaluate_score()
{
    auto& rule_body = target_rule->get_target_rule_body();
    unsigned int rule_body_size = rule_body.size();
    std::vector<const std::string*> ngram;
    unsigned int nonterm_indx = 0;
    unsigned int lm_order = lm::get_language_model_order();
    static int count = 0;

    ngram.reserve(2 * terminal_number);

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

    //++count;
    //std::cout << "count: " << count << "\n";
    /*
    unsigned int len = lm_order - 1;
    calculate_prefix(&prefix, len);
    len = lm_order - 1;
    calculate_suffix(&suffix, len);

    for (unsigned int i = 0; i < suffix.size() / 2; i++) {
        const std::string* str1 = suffix[i];
        const std::string* str2 = suffix[suffix.size() - 1 - i];

        suffix[i] = str2;
        suffix[suffix.size() - 1 - i] = str1;
    }*/

    /*
    std::cout << "prefix: ";
    for (unsigned int i = 0; i < prefix.size(); i++)
        std::cout << *prefix[i] << " ";
    std::cout << std::endl;

    std::cout << "suffix: ";
    for (unsigned int i = 0; i < suffix.size(); i++)
        std::cout << *suffix[i] << " ";
    std::cout << std::endl;*/

    //double temp = 0.0;
    //heuristic_score = 0.0;
    //lm::ngram_probability(prefix, heuristic_score, temp);

    unsigned int size = log_linear_model.get_feature_num();

    for (unsigned int i = 0; i < size; ++i) {
        feature* fea = log_linear_model.get_feature(i);
        fea->evaluate(this);
    }

    //std::cout << heuristic_score;

    //std::cout << "lm: " << log_linear_model.get_feature(0)->get_score() << std::endl;
    //std::cout << "prefix: " << heuristic_score << std::endl;

    score = log_linear_model.get_score();
}

double hypothesis::get_score() const
{
    return score;
}

void hypothesis::set_heuristic_score(double score)
{
    heuristic_score = score;
}


double hypothesis::get_heuristic_score() const
{
    return heuristic_score;
}

double hypothesis::get_total_score() const
{
    return score + heuristic_score;
}

hypothesis* hypothesis::get_previous_hypothesis(unsigned int index) const
{
    bool reorder = target_rule->reorder_nonterminal();

    switch (index) {
    case 0:
        if (reorder)
            return hypothesis_vector->front();
        else
            return hypothesis_vector->back();
    case 1:
        if (reorder)
            return hypothesis_vector->back();
        else
            return hypothesis_vector->front();
    default:
        return nullptr;
    }
}

void hypothesis::output(std::vector<const std::string*>& s)
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

/*
 * TODO:
 * hypothesis recombination
 */
void hypothesis::recombine(hypothesis* hypo)
{
    const std::string& nbest = parameter::get_parameter("nbest");
    double nbest_num = std::stod(nbest);

    if (nbest_num == 1)
        delete hypo;
    else {
        /* merge recombined hypothesis in sorted order
        hypothesis* prev = this;
        hypothesis* ptr = hypo;

        if (this->recombined_hypothesis != nullptr && hypo->recombined_hypothesis != nullptr)
            std::cout << "example" << std::endl;

        while (ptr != nullptr) {
            hypothesis* curr = prev->recombined_hypothesis;
            if (curr == nullptr) {
                prev->recombined_hypothesis = ptr;
                break;
            }

            if (curr->get_total_score() < hypo->get_total_score()) {
                prev->recombined_hypothesis = hypo;
                ptr = hypo->recombined_hypothesis;
                hypo->recombined_hypothesis = curr;
                prev = hypo;
            } else {
                prev = curr;
            }
        }*/
        recombined_hypothesis = hypo;
    }
}

void hypothesis::set_next_hypothesis(hypothesis* hypo)
{
    next_hypothesis = hypo;
}

const hypothesis* hypothesis::get_next_hypothesis() const
{
    return next_hypothesis;
}

hypothesis* hypothesis::get_recombined_hypothesis() const
{
    return recombined_hypothesis;
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
        unsigned int nonterminal_num = len - target_rule->get_terminal_num();
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

const std::vector<const std::string*>* hypothesis::get_prefix() const
{
    return &prefix;
}

const std::vector<const std::string*>* hypothesis::get_suffix() const
{
    return &suffix;
}

hypothesis::size_type hypothesis::previous_hypothesis_number() const
{
    if (hypothesis_vector == nullptr)
        return 0;

    return hypothesis_vector->size();
}

const rule* hypothesis::get_rule() const
{
    return target_rule;
}

const feature* hypothesis::get_feature(unsigned int id)
{
    return log_linear_model.get_feature(id);
}

unsigned int hypothesis::get_terminal_number() const
{
    return terminal_number;
}
