/* derivation.cpp */
#include <rule.h>
#include <derivation.h>
#include <hypothesis.h>
#include <cstdio>
#include <verbose.h>

derivation::derivation(const hypothesis* h)
{
    initialize(h);
}

derivation::derivation(const derivation* d, const hypothesis* substituted,
    const hypothesis* replacement)
{
    generated_hypothesis = nullptr;

    if (d->hypo == substituted) {
        initialize(replacement);
        return;
    }

    hypo = d->hypo;
    score = 0.0;

    if (d->back_pointer == nullptr) {
        back_pointer = nullptr;
        return;
    }

    size_type size = d->back_pointer->size();
    back_pointer = new std::vector<derivation*>();
    back_pointer->reserve(size);

    for (size_type i = 0; i < size; i++) {
        const derivation* prev = d->back_pointer->at(i);
        derivation* new_d = new derivation(prev, substituted, replacement);
        back_pointer->push_back(new_d);
    }
}

derivation::~derivation()
{
    if (generated_hypothesis != nullptr)
        delete generated_hypothesis;

    if (back_pointer == nullptr)
        return;

    unsigned int size = back_pointer->size();

    for (unsigned int i = 0; i < size; i++) {
        derivation* d = back_pointer->at(i);
        delete d;
    }

    delete back_pointer;
}

void derivation::initialize(const hypothesis* h)
{
    hypo = h;
    score = 0.0;
    generated_hypothesis = nullptr;
    size_type size = hypo->previous_hypothesis_number();

    if (!size) {
        back_pointer = nullptr;
        return;
    } else {
        back_pointer = new std::vector<derivation*>();
        back_pointer->reserve(size);
    }

    std::vector<hypothesis*>* vec = hypo->get_hypothesis_vector();

    for (size_type i = 0; i < size; i++) {
        const hypothesis* h = vec->at(i);
        back_pointer->push_back(new derivation(h));
    }
}

double derivation::get_score() const
{
    return score;
}

void derivation::generate_hypothesis()
{
    if (generated_hypothesis != nullptr)
        return;

    const rule* r = hypo->get_rule();
    generated_hypothesis = new hypothesis(r);

    if (back_pointer == nullptr) {
        generated_hypothesis->evaluate_score();
        return;
    }

    size_type size = back_pointer->size();

    for (size_type i = 0; i < size; i++) {
        derivation* d = back_pointer->at(i);
        hypothesis* prev;
        d->generate_hypothesis();
        prev = const_cast<hypothesis*>(d->get_generated_hypothesis());
        generated_hypothesis->push_hypothesis(prev);
    }

    generated_hypothesis->evaluate_score();
    score = generated_hypothesis->get_total_score();
}

const hypothesis* derivation::get_hypothesis() const
{
    return hypo;
}

const hypothesis* derivation::get_generated_hypothesis() const
{
    return generated_hypothesis;
}

unsigned int derivation::sub_derivation_number() const
{
    if (back_pointer == nullptr)
        return 0;

    return back_pointer->size();
}
const derivation* derivation::get_sub_derivation(unsigned int i) const
{
    const rule* r = hypo->get_rule();
    bool reorder = r->reorder_nonterminal();

    if (reorder) {
        if (i == 0)
            return back_pointer->at(1);
        else
            return back_pointer->at(0);
    } else {
        return back_pointer->at(i);
    }
}

void derivation::output(std::vector<const std::string*>& ret)
{
    if (generated_hypothesis == nullptr)
        generate_hypothesis();
    generated_hypothesis->output(ret);
}
