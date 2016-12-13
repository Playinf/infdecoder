/* trellis.cpp */
#include <rule.h>
#include <symbol.h>
#include <trellis.h>
#include <hypothesis.h>

// trellis_node
trellis_node::trellis_node(const hypothesis* hypo)
{
    this->hypo = hypo;
    create_children();
}

trellis_node::trellis_node(const trellis_detour* detour,
    trellis_node** deviation_node)
{
    const trellis_path* base_path = detour->get_base_path();
    const trellis_node* final_node = base_path->get_final_node();
    const trellis_node* substituted_node = detour->get_substituted_node();
    const hypothesis* replacement_hypo = detour->get_replacement_hypothesis();

    if (final_node == substituted_node) {
        hypo = replacement_hypo;
        *deviation_node = this;
        create_children();
    } else {
        hypo = final_node->get_hypothesis();
        create_children(final_node, substituted_node, replacement_hypo,
            deviation_node);
    }
}

trellis_node::trellis_node(const trellis_node* root,
    const trellis_node* substituted, const hypothesis* replacement,
    trellis_node** deviation_node)
{
    if (root == substituted) {
        hypo = replacement;
        *deviation_node = this;
        create_children();
    } else {
        hypo = root->get_hypothesis();
        create_children(root, substituted, replacement, deviation_node);
    }
}

trellis_node::~trellis_node()
{
    size_t size = children.size();

    for (size_t i = 0; i < size; i++) {
        trellis_node* node = children[i];
        delete node;
    }

    children.clear();
}

trellis_node::size_type trellis_node::get_children_num() const
{
    return children.size();
}

const hypothesis* trellis_node::get_hypothesis() const
{
    return hypo;
}

const trellis_node* trellis_node::get_child(size_type index) const
{
    return children[index];
}

void trellis_node::output(std::vector<const std::string*>* sentence) const
{
    const rule* target_rule = hypo->get_rule();
    auto& rule_body = target_rule->get_target_rule_body();
    unsigned int size = rule_body.size();
    unsigned int nonterminal_index = 0;

    for (unsigned int i = 0; i < size; i++) {
        const symbol* sym = rule_body[i];

        if (sym->is_terminal())
            sentence->push_back(sym->get_name());
        else {
            const trellis_node* n = children[nonterminal_index];
            ++nonterminal_index;
            n->output(sentence);
        }
    }
}

void trellis_node::create_children()
{
    size_t size = hypo->get_previous_hypothesis_number();

    if (!size)
        return;

    children.reserve(size);

    // recursively create child and push into vector
    for (size_t i = 0; i < size; i++) {
        const hypothesis* h = hypo->get_previous_hypothesis(i);
        trellis_node* node = new trellis_node(h);
        children.push_back(node);
    }
}

void trellis_node::create_children(const trellis_node* root,
    const trellis_node* substituted, const hypothesis* replacement,
    trellis_node** deviation_node)
{
    size_type size = root->get_children_num();

    children.reserve(size);

    for (size_type i = 0; i < size; i++) {
        const trellis_node* node = root->get_child(i);
        trellis_node* new_node = new trellis_node(node, substituted,
            replacement, deviation_node);
        children.push_back(new_node);
    }
}

// trellis_path
trellis_path::trellis_path(const hypothesis* hypo)
{
    unsigned int feature_num = hypo->get_feature_number();

    total_score = hypo->get_total_score();
    score_vector.reserve(feature_num);

    for (unsigned int i = 0; i < feature_num; i++) {
        const feature* f = hypo->get_feature(i);
        score_vector.push_back(f->get_score());
    }

    final_node = new trellis_node(hypo);
    deviation_node = nullptr;
}

trellis_path::trellis_path(const trellis_detour* detour)
{
    const trellis_path* basepath = detour->get_base_path();
    const trellis_node* substituted_node = detour->get_substituted_node();
    const hypothesis* substituted = substituted_node->get_hypothesis();
    const hypothesis* replacement = detour->get_replacement_hypothesis();
    const std::vector<float>* base_score_vector = basepath->get_score_vector();
    unsigned int size = base_score_vector->size();

    score_vector.reserve(size);

    for (unsigned int i = 0; i < size; i++) {
        const feature* f1 = substituted->get_feature(i);
        const feature* f2 = replacement->get_feature(i);
        float score = base_score_vector->at(i);
        score += f2->get_score() - f1->get_score();
        score_vector.push_back(score);
    }

    total_score = detour->get_total_score();

    final_node = new trellis_node(detour, &deviation_node);
}

trellis_path::~trellis_path()
{
    delete final_node;
}

float trellis_path::get_total_score() const
{
    return total_score;
}

const trellis_node* trellis_path::get_final_node() const
{
    return final_node;
}

const trellis_node* trellis_path::get_deviation_node() const
{
    return deviation_node;
}

const std::vector<float>* trellis_path::get_score_vector() const
{
    return &score_vector;
}

void trellis_path::output(std::vector<const std::string*>* sentence) const
{
    final_node->output(sentence);
}

// trellis_detour
trellis_detour::trellis_detour(std::shared_ptr<const trellis_path> base_path,
        const trellis_node* substituted, const hypothesis* replacement)
{
    const hypothesis* hypo = substituted->get_hypothesis();
    float diff = hypo->get_total_score() - replacement->get_total_score();

    this->base_path = base_path;
    this->substituted_node = substituted;
    this->replacement_hypothesis = replacement;
    this->total_score = base_path->get_total_score() - diff;
}

trellis_detour::~trellis_detour()
{
    // do nothing
}

const trellis_path* trellis_detour::get_base_path() const
{
    return base_path.get();
}

const trellis_node* trellis_detour::get_substituted_node() const
{
    return substituted_node;
}

const hypothesis* trellis_detour::get_replacement_hypothesis() const
{
    return replacement_hypothesis;
}

float trellis_detour::get_total_score() const
{
    return total_score;
}
