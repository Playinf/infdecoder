/* cube_item.cpp */
#include <utility.h>
#include <cube_item.h>

cube_item::cube_item(rule_list* list)
    : rule_position(list->get_rule_vector(), 0)
{
    unsigned num = list->get_rule_nonterminal_number();
    generated_hypothesis = nullptr;

    if (num) {
        hypothesis_position = new std::vector<hypothesis_dimension>;

        for (unsigned int i = 0; i < num; i++) {
            auto hypo_vec = list->get_hypothesis_vector(i);
            hypothesis_dimension dim(hypo_vec, 0);
            hypothesis_position->push_back(dim);
        }
    } else
        hypothesis_position = nullptr;
}

cube_item::cube_item(const cube_item& item, unsigned int dim_index)
    : rule_position(item.rule_position)
{
    generated_hypothesis = nullptr;

    if (item.hypothesis_position) {
        hypothesis_position = new std::vector<hypothesis_dimension>;
        unsigned int size = item.hypothesis_position->size();

        for (unsigned int i = 0; i < size; i++)
            hypothesis_position->push_back(item.hypothesis_position->at(i));
    } else
        hypothesis_position = nullptr;

    if (dim_index == -1)
        rule_position.next_rule();
    else {
        auto& hypo_pos = hypothesis_position->at(dim_index);
        hypo_pos.next_hypothesis();
    }
}

#include <verbose.h>

cube_item::~cube_item()
{
    if (hypothesis_position != nullptr)
        delete hypothesis_position;

    if (generated_hypothesis != nullptr) {
        delete generated_hypothesis;
    }
}

double cube_item::get_score() const
{
    return score;
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

            hypo->push_hypothesis(h);
        }
    }

    hypo->evaluate_score();
    generated_hypothesis = hypo;
    score = hypo->get_total_score();
}

hypothesis* cube_item::get_hypothesis()
{
    hypothesis* hypo = generated_hypothesis;
    generated_hypothesis = nullptr;

    return hypo;
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
    auto hypo_dim1 = hypothesis_position;
    auto hypo_dim2 = item.hypothesis_position;

    if (rule_position != item.rule_position)
        return false;

    if (hypo_dim1 != nullptr && hypo_dim2 != nullptr) {
        unsigned int size1 = hypo_dim1->size();
        unsigned int size2 = hypo_dim2->size();

        if (size1 != size2)
            return false;

        for (unsigned int i = 0; i < size1; i++) {
            hypothesis_dimension& dim1 = hypo_dim1->at(i);
            hypothesis_dimension& dim2 = hypo_dim2->at(i);

            if (dim1 != dim2)
                return false;
        }

        return true;
    } else if (hypo_dim1 == hypo_dim2)
        return true;

    return false;
}

unsigned int cube_item::hash() const
{
    const char* p = reinterpret_cast<const char*>(&rule_position);
    unsigned int size = sizeof(rule_position);
    unsigned int hash_code = data_hash(p, size);

    if (!hypothesis_position)
        return hash_code;

    for (unsigned int i = 0; i < hypothesis_position->size(); i++) {
        hypothesis_dimension* dim = &hypothesis_position->at(i);
        p = reinterpret_cast<const char*>(dim);
        unsigned int size = sizeof(hypothesis_dimension);

        hash_code += data_hash(p, size);
    }

    return size;
}
