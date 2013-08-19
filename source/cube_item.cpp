/* cube_item.cpp */
#include <functional>
#include <rule.h>
#include <utility.h>
#include <alignment.h>
#include <cube_item.h>
#include <hypothesis.h>
#include <translation_option.h>

rule_dimension::rule_dimension(std::vector<const rule*>* rules,
    unsigned int pos)
{
    sorted_rule_list = rules;
    position = pos;
}

rule_dimension::~rule_dimension()
{
    /* do nothing */
}

const rule* rule_dimension::get_rule() const
{
    return sorted_rule_list->at(position);
}

unsigned int rule_dimension::get_position() const
{
    return position;
}

bool rule_dimension::has_more_rules() const
{
    unsigned int rule_num = sorted_rule_list->size();

    if (position < rule_num - 1)
        return true;

    return false;
}

void rule_dimension::next_rule()
{
    position += 1;
}

hypothesis_dimension::hypothesis_dimension(std::vector<hypothesis*>* hypo,
    unsigned int num)
{
    sorted_hypothesis_list = hypo;
    position = num;
}

hypothesis_dimension::~hypothesis_dimension()
{
    /* do nothing */
}

unsigned int hypothesis_dimension::get_position() const
{
    return position;
}

const hypothesis* hypothesis_dimension::get_hypothesis() const
{
    return sorted_hypothesis_list->at(position);
}

bool hypothesis_dimension::has_more_hypothesis() const
{
    unsigned int hypo_num = sorted_hypothesis_list->size();

    if (position < hypo_num - 1)
        return true;

    return false;
}

void hypothesis_dimension::next_hypothesis()
{
    position += 1;
}

cube_item::cube_item(translation_option* option)
    : rule_position(option->get_rule_vector(), 0)
{
    unsigned int num = option->get_rule_nonterminal_number();
    generated_hypothesis = nullptr;

    if (num) {
        hypothesis_position = new std::vector<hypothesis_dimension>;
        hypothesis_position->reserve(num);

        for (unsigned int i = 0; i < num; i++) {
            auto hypo_vec = option->get_hypothesis_vector(i);
            hypothesis_dimension dim(hypo_vec, 0);
            hypothesis_position->push_back(dim);
        }
    } else {
        hypothesis_position = nullptr;
    }
}

cube_item::cube_item(const cube_item& item, unsigned int dim_index)
    : rule_position(item.rule_position)
{
    const unsigned int rule_dim = 0;

    generated_hypothesis = nullptr;

    if (item.hypothesis_position != nullptr) {
        hypothesis_position = new std::vector<hypothesis_dimension>;
        unsigned int size = item.hypothesis_position->size();

        hypothesis_position->reserve(size);

        for (unsigned int i = 0; i < size; i++)
            hypothesis_position->push_back(item.hypothesis_position->at(i));
    } else {
        hypothesis_position = nullptr;
    }

    if (dim_index == rule_dim) {
        rule_position.next_rule();
    } else {
        unsigned int index = dim_index - 1;
        auto& hypo_pos = hypothesis_position->at(index);
        hypo_pos.next_hypothesis();
    }
}

cube_item::~cube_item()
{
    if (hypothesis_position != nullptr)
        delete hypothesis_position;

    if (generated_hypothesis != nullptr)
        delete generated_hypothesis;
}

float cube_item::get_score() const
{
    return score;
}

hypothesis* cube_item::get_hypothesis()
{
    hypothesis* hypo = generated_hypothesis;
    generated_hypothesis = nullptr;

    return hypo;
}

unsigned int cube_item::get_rule_position() const
{
    return rule_position.get_position();
}

unsigned int cube_item::get_hypothesis_dimension() const
{
    if (hypothesis_position == nullptr)
        return 0;

    return hypothesis_position->size();
}

unsigned int cube_item::get_hypothesis_position(unsigned int dim) const
{
    hypothesis_dimension& h = hypothesis_position->at(dim);

    return h.get_position();
}

void cube_item::generate_hypothesis()
{
    const rule* r = rule_position.get_rule();
    hypothesis* hypo = new hypothesis(const_cast<rule*>(r));

    if (hypothesis_position != nullptr) {
        unsigned int size = hypothesis_position->size();

        for (unsigned int i = 0; i < size; i++) {
            hypothesis_dimension& dim = hypothesis_position->at(i);
            hypothesis* h = const_cast<hypothesis*>(dim.get_hypothesis());
            const alignment* align = r->get_alignment();
            unsigned int index = align->get_nonterminal_map(i);

            hypo->set_previous_hypothesis(index, h);
        }
    }

    hypo->evaluate_score();
    generated_hypothesis = hypo;
    score = hypo->get_total_score();
}

bool cube_item::has_more_rules() const
{
    return rule_position.has_more_rules();
}

bool cube_item::has_more_hypotheses(unsigned int dim) const
{
    if (hypothesis_position == nullptr)
        return false;

    return hypothesis_position->at(dim).has_more_hypothesis();
}

bool cube_item::operator==(const cube_item& item) const
{
    unsigned int rule_pos1 = rule_position.get_position();
    unsigned int rule_pos2 = item.rule_position.get_position();

    if (rule_pos1 != rule_pos2)
        return false;

    auto hypo_dim1 = hypothesis_position;
    auto hypo_dim2 = item.hypothesis_position;

    if (hypo_dim1 != nullptr && hypo_dim2 != nullptr) {
        unsigned int size1 = hypo_dim1->size();
        unsigned int size2 = hypo_dim2->size();

        if (size1 != size2)
            return false;

        for (unsigned int i = 0; i < size1; i++) {
            hypothesis_dimension& dim1 = hypo_dim1->at(i);
            hypothesis_dimension& dim2 = hypo_dim2->at(i);
            unsigned int hypo_pos1 = dim1.get_position();
            unsigned int hypo_pos2 = dim2.get_position();

            if (hypo_pos1 != hypo_pos2)
                return false;
        }

        return true;
    } else if (hypo_dim1 == hypo_dim2)
        return true;

    return false;
}

unsigned int cube_item::hash() const
{
    std::hash<unsigned int> hasher;
    unsigned int rule_pos = rule_position.get_position();
    unsigned int hash_code;

    hash_code = hasher(rule_pos);

    if (!hypothesis_position)
        return hash_code;

    for (unsigned int i = 0; i < hypothesis_position->size(); i++) {
        hypothesis_dimension* dim = &hypothesis_position->at(i);
        unsigned int hypo_pos = dim->get_position();
        unsigned int hash_val = 0;

        hash_val = hasher(hypo_pos);
        hash_code = hash_combine(hash_code, hash_val);
    }

    return hash_code;
}
