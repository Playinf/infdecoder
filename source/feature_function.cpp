/* feature_function.cpp */
#include <utility>
#include <stdexcept>
#include <rule.h>
#include <model.h>
#include <config.h>
#include <symbol.h>
#include <feature.h>
#include <utility.h>
#include <jm_state.h>
#include <lm_state.h>
#include <alignment.h>
#include <rule_tree.h>
#include <chart_cell.h>
#include <hypothesis.h>
#include <information.h>
#include <joint_model.h>
#include <partial_rule.h>
#include <lexical_model.h>
#include <penalty_model.h>
#include <language_model.h>
#include <translation_model.h>
#include <ngram_language_model.h>

// for debug
//#include <iostream>
//#include <verbose.h>

// defined in standard.cpp
void output_hypothesis(const hypothesis*, std::vector<const std::string*>&);
/*
void print_ngram(std::vector<const std::string*> ngram)
{
    int size = ngram.size();

    for (int i = 0; i < size; i++) {
        std::cout << *ngram[i] << " ";
    }
}*/

// exchange <s> and </s>, used in backward language model
void transform(std::vector<const std::string*>& vec)
{
    unsigned int size = vec.size();

    if (size == 0)
        return;

    if (*vec[size - 1] == "<s>") {
        symbol_table* symtab = symbol_table::get_instance();
        const symbol* eos = symtab->search_symbol("</s>");
        vec[size - 1] = eos->get_name();
    } else if (*vec[0] == "</s>") {
        symbol_table* symtab = symbol_table::get_instance();
        const symbol* bos = symtab->search_symbol("<s>");
        vec[0] = bos->get_name();
    }
}

static void get_source_rule(const partial_rule* r,
    std::vector<const symbol*>& v)
{
    const rt_node* node = r->get_node();
    const symbol* src_sym = node->get_symbol(0);
    const partial_rule* parent_rule = r->get_parent();

    if (parent_rule != nullptr) {
        get_source_rule(parent_rule, v);
    }

    if (src_sym != nullptr) {
        v.push_back(src_sym);
    }
}

static void get_translation(const hypothesis* h,
    std::vector<const std::string*>& v, unsigned int id)
{
    const rule* r = h->get_rule();
    auto& rule_body = r->get_target_rule_body();
    unsigned int rule_body_size = rule_body.size();
    unsigned int indx = 0;

    for (unsigned int i = 0; i < rule_body_size; i++) {
        const symbol* sym = rule_body[i];

        if (sym->is_terminal()) {
            v.push_back(sym->get_name());
        } else {
            hypothesis* prev_hypo = h->get_previous_hypothesis(indx++);
            const feature* prev_fea = prev_hypo->get_feature(id);
            state* prev_state = prev_fea->get_state();
            jm_state* prev_jm_state = static_cast<jm_state*>(prev_state);
            const auto& prev_trans = prev_jm_state->get_translation();
            auto iter_begin = prev_trans->begin();
            auto iter_end = prev_trans->end();

            for (auto iter = iter_begin; iter != iter_end; ++iter)
                v.push_back(*iter);
        }
    }
}

static void get_affiliation(const hypothesis* h, std::vector<unsigned int>& v,
    unsigned int id)
{
    const rule* r = h->get_rule();
    const partial_rule* srule = h->get_partial_rule();
    auto& rule_body = r->get_target_rule_body();
    const alignment* align = r->get_alignment();
    unsigned int rule_body_size = rule_body.size();
    unsigned int source_rule_size = srule->get_length();
    auto span = h->get_span();
    unsigned int indx = 0;
    unsigned int source_index;

    // get target alignment
    std::vector<std::vector<unsigned int>> tgt_align_vec;
    tgt_align_vec.resize(rule_body_size);
    auto& full_align = align->get_alignment();
    unsigned int full_align_size = full_align.size();

    //std::cout << "source: ";
    //print_partial_rule(srule);
    //std::cout << "target: ";
    //print_rule(r);
    //std::cout << "alignment: ";

    for (unsigned int i = 0; i < full_align_size; i++) {
        auto& align_pair = full_align[i];
        tgt_align_vec[align_pair.second].push_back(align_pair.first);
        //std::cout << align_pair.first << "-" << align_pair.second << " ";
    }

    //std::cout << std::endl;

    // get absolute position of source symbol
    std::vector<unsigned int> pos_vec(source_rule_size, 0);
    pos_vec[0] = span.first;

    for (unsigned int i = 0; i < rule_body_size; i++) {
        const symbol* sym = rule_body[i];

        if (sym->is_nonterminal()) {
            unsigned int tntid = align->get_target_nonterminal_index(i);
            hypothesis* prev_hypo = h->get_previous_hypothesis(tntid);
            auto pspan = prev_hypo->get_span();
            unsigned int sntid;
            unsigned int index;

            sntid = align->get_nonterminal_map(tntid, direction::target);
            index = align->get_source_nonterminal_position(sntid);

            pos_vec[index] = pspan.second;
        }
    }

    for (unsigned int i = 0; i < source_rule_size; i++) {
        if (i && pos_vec[i] == 0) {
            pos_vec[i] = pos_vec[i - 1] + 1;
        }
    }

    // find all affiliation of rule
    for (unsigned int i = 0; i < rule_body_size; i++) {
        const symbol* sym = rule_body[i];

        if (sym->is_nonterminal()) {
            hypothesis* prev_hypo = h->get_previous_hypothesis(indx++);
            const feature* prev_fea = prev_hypo->get_feature(id);
            state* prev_state = prev_fea->get_state();
            jm_state* prev_jm_state = static_cast<jm_state*>(prev_state);
            const auto& prev_affiliation = prev_jm_state->get_affiliation();
            auto iter_begin = prev_affiliation->begin();
            auto iter_end = prev_affiliation->end();

            for (auto iter = iter_begin; iter != iter_end; ++iter)
                v.push_back(*iter);
        } else {
            bool found = false;
            const std::vector<unsigned int>* align_vec = &tgt_align_vec[i];

            if (align_vec->size() == 0) {
                for (unsigned int len = 1; len < rule_body_size; len++) {
                    int pos1 = i + len;
                    int pos2 = i - len;
                    // right side
                    if (pos1 < static_cast<int>(rule_body_size)) {
                        const symbol* s = rule_body[pos1];
                        if (s->is_nonterminal()) {
                            jm_state* pstate;
                            hypothesis* phypo;
                            const feature* fea;
                            unsigned int ntid;

                            ntid = align->get_target_nonterminal_index(pos1);
                            phypo = h->get_previous_hypothesis(ntid);
                            fea = phypo->get_feature(id);
                            pstate = static_cast<jm_state*>(fea->get_state());
                            const auto* pavec = pstate->get_affiliation();

                            if (pavec->size() != 0) {
                                source_index = pavec->front();
                                found = true;
                                break;
                            }
                        } else {
                            align_vec = &tgt_align_vec[pos1];

                            if (align_vec->size() != 0)
                                break;
                        }
                    }
                    // left side
                    if (pos2 >= 0) {
                        const symbol* s = rule_body[pos2];
                        if (s->is_nonterminal()) {
                            jm_state* pstate;
                            hypothesis* phypo;
                            const feature* fea;
                            unsigned int ntid;

                            ntid = align->get_target_nonterminal_index(pos2);
                            phypo = h->get_previous_hypothesis(ntid);
                            fea = phypo->get_feature(id);
                            pstate = static_cast<jm_state*>(fea->get_state());
                            const auto* pavec = pstate->get_affiliation();

                            if (pavec->size() != 0) {
                                source_index = pavec->back();
                                found = true;
                                break;
                            }
                        } else {
                            align_vec = &tgt_align_vec[pos2];

                            if (align_vec->size() != 0)
                                break;
                        }
                    }

                    if (pos1 >= static_cast<int>(rule_body_size) && pos2 < 0)
                        break;
                }
            }

            if (!found) {
                if (align_vec->size() == 0)
                    throw std::runtime_error("no affiliation found");
                unsigned int index = (*align_vec)[align_vec->size() / 2];
                source_index = pos_vec[index];
            }

            v.push_back(source_index);
        }
    }
}

static void probability(std::vector<const std::string*>& ngram,
    std::vector<unsigned int>& avec, const std::vector<const symbol*>& input,
    joint_model* jm, float& fs, float& ns)
{
    unsigned int sctx = jm->get_source_context();
    unsigned int tctx = jm->get_target_context();
    unsigned int input_size = input.size();
    unsigned int size = ngram.size();
    symbol_table* symtab = symbol_table::get_instance();
    const symbol* bos = symtab->search_symbol("<s>");
    const symbol* eos = symtab->search_symbol("</s>");
    const symbol* null = symtab->search_symbol("<null>");
    bool begin_sentence = false;

    //for (unsigned int i = 0; i < ngram.size(); i++) {
    //    std::cout << *ngram[i] << " ";
    //}
    //std::cout << std::endl;

    if (size > 0 && ngram[0] == bos->get_name()) {
        begin_sentence = true;
    }

    for (unsigned int i = 0; i < size; i++) {
        unsigned int sid = avec[i];
        std::vector<const std::string*> vec;
        unsigned int right_window = sctx / 2;
        unsigned int left_window = sctx - right_window - 1;

        vec.reserve(sctx + tctx + 1);

        // get source context, padding <s> and </s>
        for (unsigned int j = 0; j < left_window; j++) {
            int index = sid - left_window + j;
            if (index > 0)
                vec.push_back(input[index]->get_name());
            else
                vec.push_back(bos->get_name());
        }

        vec.push_back(input[sid]->get_name());

        for (unsigned int j = 0; j < right_window; j++) {
            unsigned int index = sid + j + 1;
            if (index >= input_size)
                vec.push_back(eos->get_name());
            else
                vec.push_back(input[index]->get_name());
        }

        // get target context, padding <null>
        for (unsigned int j = 0; j < tctx; j++) {
            int index = i - tctx + j;
            if (index < 0) {
                if (begin_sentence)
                    vec.push_back(bos->get_name());
                else
                    vec.push_back(null->get_name());
            } else {
                vec.push_back(ngram[index]);
            }
        }

        vec.push_back(ngram[i]);

        double score = jm->probability(vec.data());
        fs += score;

        // a full n-gram
        if (i >= tctx) {
            ns += score;
        }
    }
}

static float forward_jm_feature_function(const hypothesis* h, unsigned int id)
{
    bool calc = false;
    bool prefix = true;
    float full = 0.0f;
    float score = 0.0f;
    float prev_score = 0.0f;
    float prefix_score = 0.0f;
    const rule* r = h->get_rule();
    information* info = h->get_input();
    auto& rule_body = r->get_target_rule_body();
    unsigned int rule_body_size = rule_body.size();
    configuration* config = configuration::get_instance();
    model* system_model = config->get_model();
    language_model* lm = system_model->get_language_model(id);
    joint_model* jm = static_cast<joint_model*>(lm);
    feature* lm_feature = const_cast<feature*>(h->get_feature(id));
    float old_prefix_score = 0.0f;
    unsigned int target_order = jm->get_target_context() + 1;
    unsigned int indx = 0;
    unsigned int offset = 0;
    std::vector<unsigned int> affiliation;
    std::vector<const std::string*> ngram;
    std::vector<unsigned int> ngram_affiliation;
    std::vector<const std::string*> translation;
    const std::vector<const symbol*>& input = *info->get_sentence();
    unsigned int tnum = h->get_terminal_number();

    affiliation.reserve(tnum);
    translation.reserve(tnum);

    get_translation(h, translation, id);
    get_affiliation(h, affiliation, id);

    //std::cout << "translation: ";
    //for (unsigned int i = 0; i < translation.size(); i++) {
    //    std::cout << *translation[i] << " ";
    //}
    //std::cout << std::endl;

    //for (unsigned int i = 0; i < affiliation.size(); i++) {
    //    std::cout << i << "-" << affiliation[i] << " ";
    //}
    //std::cout << std::endl;

    //for (unsigned int i = 0; i < rule_body_size; i++) {
    //    std::cout << *rule_body[i]->get_name() << " ";
    //}
    //std::cout << std::endl;

    for (unsigned int i = 0; i < rule_body_size; i++) {
        const symbol* sym = rule_body[i];

        if (sym->is_terminal()) {
            ngram.push_back(sym->get_name());
            ngram_affiliation.push_back(affiliation[offset]);
            offset++;
        } else {
            hypothesis* prev_hypo = h->get_previous_hypothesis(indx++);
            unsigned int prev_term_num = prev_hypo->get_terminal_number();
            const feature* prev_fea = prev_hypo->get_feature(id);
            state* prev_state = prev_fea->get_state();
            jm_state* prev_jm_state = static_cast<jm_state*>(prev_state);
            unsigned int begin;
            unsigned int end;
            unsigned int length;

            prev_score += prev_fea->get_score();
            old_prefix_score += prev_jm_state->get_score();

            if (prev_term_num >= target_order - 1) {
                calc = true;
                length = target_order - 1;
            } else {
                length = prev_term_num;
            }

            begin = offset;
            end = begin + length;

            for (unsigned int i = begin; i < end; i++) {
                ngram.push_back(translation[i]);
                ngram_affiliation.push_back(affiliation[i]);
            }

            if (calc) {
                if (i == 0) {
                    prefix_score = prev_jm_state->get_score();
                } else {
                    float ns = 0.0;
                    float fs = 0.0;
                    probability(ngram, ngram_affiliation, input, jm, fs, ns);

                    if (prefix)
                        prefix_score = fs - ns;
                    full += fs;
                    score += ns;
                }

                end = offset + prev_term_num;
                begin = end - length;
                offset = end;

                ngram.clear();
                ngram_affiliation.clear();

                for (unsigned int i = begin; i < end; i++) {
                    ngram.push_back(translation[i]);
                    ngram_affiliation.push_back(affiliation[i]);
                }
                calc = false;
                prefix = false;
            } else {
                offset = end;
            }
        }
    }

    float full_score = 0.0;
    float ngram_score = 0.0;
    probability(ngram, ngram_affiliation, input, jm, full_score, ngram_score);

    if (prefix)
        prefix_score = full_score - ngram_score;

    score += ngram_score;
    full += full_score;
    score += prev_score;
    score -= old_prefix_score;
    jm_state* state = new jm_state(h, id);
    state->calculate_prefix_suffix(target_order);
    state->set_score(prefix_score);
    state->set_affiliation(affiliation);
    state->set_translation(translation);
    lm_feature->add_state(state);
    score += prefix_score;

    return score;
}

/*
static float forward_jm_feature_function(const hypothesis* h, unsigned int id)
{
    float score = 0.0f;
    const rule* r = h->get_rule();
    information* info = h->get_input();
    auto& rule_body = r->get_target_rule_body();
    configuration* config = configuration::get_instance();
    model* system_model = config->get_model();
    language_model* lm = system_model->get_language_model(id);
    joint_model* jm = static_cast<joint_model*>(lm);
    feature* lm_feature = const_cast<feature*>(h->get_feature(id));
    unsigned int source_context = jm->get_source_context();
    unsigned int target_context = jm->get_target_context();
    const std::vector<const symbol*>& input = *info->get_sentence();
    std::vector<unsigned int> affiliation;
    std::vector<const std::string*> translation;
    unsigned int tnum = h->get_terminal_number();
    std::vector<const std::string*> ngram;

    affiliation.reserve(tnum);
    translation.reserve(tnum);

    get_affiliation(h, affiliation, id);
    get_translation(h, translation, id);

    //std::cerr << "translation:\n";
    //for (unsigned int i = 0; i < translation.size(); i++) {
    //    std::cerr << *translation[i] << " ";
    //}
    //std::cerr << std::endl;

    ngram.reserve(source_context + target_context + 1);

    symbol_table* symtab = symbol_table::get_instance();
    const symbol* bos = symtab->search_symbol("<s>");
    const symbol* eos = symtab->search_symbol("</s>");
    const symbol* null = symtab->search_symbol("<null>");
    unsigned int right_window = source_context / 2;
    unsigned int left_window = source_context - right_window - 1;
    unsigned int input_size = input.size();
    bool begin_sentence;

    if (translation[0] == bos->get_name())
        begin_sentence = true;
    else
        begin_sentence = false;

    for (unsigned int i = 0; i < tnum; i++) {
        unsigned int sid = affiliation[i];

        // append source context
        for (unsigned int j = 0; j < left_window; j++) {
            int pos = sid - left_window + j;

            if (pos < 0) {
                ngram.push_back(bos->get_name());
            } else {
                ngram.push_back(input[pos]->get_name());
            }
        }

        ngram.push_back(input[sid]->get_name());

        for (unsigned int j = 0; j < right_window; j++) {
            unsigned int pos = sid + 1 + j;

            if (pos >= input_size) {
                ngram.push_back(eos->get_name());
            } else {
                ngram.push_back(input[pos]->get_name());
            }
        }

        // append target context

        for (unsigned int j = 0; j < target_context; j++) {
            int pos = i - target_context + j;

            if (pos < 0) {
                if (begin_sentence) {
                    ngram.push_back(bos->get_name());
                } else {
                    ngram.push_back(null->get_name());
                }
            } else {
                ngram.push_back(translation[pos]);
            }
        }

        ngram.push_back(translation[i]);

        //for (unsigned int j = 0; j < ngram.size(); j++) {
        //    std::cerr << *ngram[j] << " ";
        //}
        //std::cerr << std::endl;

        score += jm->probability(ngram.data());

        ngram.clear();
    }

    jm_state* state = new jm_state(h, id);
    state->calculate_prefix_suffix(target_context + 1);
    state->set_affiliation(affiliation);
    state->set_translation(translation);
    lm_feature->add_state(state);

    return score;
}*/

static float backward_jm_feature_function(const hypothesis* h, unsigned int id)
{
    throw std::runtime_error("backward_jm_feature_function not implemented");
}

static float forward_lm_feature_function(const hypothesis* h, unsigned int id)
{
    bool calc = false;
    bool prefix = true;
    float full = 0.0f;
    float score = 0.0f;
    float prev_score = 0.0f;
    float prefix_score = 0.0f;
    const rule* r = h->get_rule();
    auto& rule_body = r->get_target_rule_body();
    unsigned int rule_body_size = rule_body.size();
    configuration* config = configuration::get_instance();
    model* system_model = config->get_model();
    language_model* lm = system_model->get_language_model(id);
    ngram_language_model* nlm = static_cast<ngram_language_model*>(lm);
    feature* lm_feature = const_cast<feature*>(h->get_feature(id));
    float old_prefix_score = 0.0f;
    unsigned int lm_order = nlm->get_order();
    unsigned int indx = 0;
    std::vector<const std::string*> ngram;

    for (unsigned int i = 0; i < rule_body_size; i++) {
        const symbol* sym = rule_body[i];

        if (sym->is_terminal()) {
            ngram.push_back(sym->get_name());
        } else {
            hypothesis* prev_hypo = h->get_previous_hypothesis(indx++);
            unsigned int prev_term_num = prev_hypo->get_terminal_number();
            const feature* prev_fea = prev_hypo->get_feature(id);
            state* prev_state = prev_fea->get_state();
            lm_state* prev_lm_state = static_cast<lm_state*>(prev_state);
            auto hypo_prefix = prev_lm_state->get_prefix();
            auto hypo_suffix = prev_lm_state->get_suffix();
            auto iter_begin = hypo_prefix->begin();
            auto iter_end = hypo_prefix->end();

            prev_score += prev_fea->get_score();
            old_prefix_score += prev_lm_state->get_score();
            ngram.insert(ngram.end(), iter_begin, iter_end);

            if (prev_term_num >= lm_order - 1) {
                calc = true;
            }

            if (calc) {
                iter_begin = hypo_suffix->begin();
                iter_end = hypo_suffix->end();

                if (i == 0 && prev_term_num >= lm_order - 1) {
                    prefix_score = prev_lm_state->get_score();
                } else {
                    float ngram_score = 0.0;
                    float full_score = 0.0;
                    nlm->probability(ngram, full_score, ngram_score);

                    if (prefix)
                        prefix_score = full_score - ngram_score;
                    full += full_score;
                    score += ngram_score;
                }

                ngram.clear();
                ngram.insert(ngram.end(), iter_begin, iter_end);
                calc = false;
                prefix = false;
            }
        }
    }

    float full_score = 0.0;
    float ngram_score = 0.0;
    nlm->probability(ngram, full_score, ngram_score);

    if (prefix)
        prefix_score = full_score - ngram_score;

    score += ngram_score;
    full += full_score;
    score += prev_score;
    score -= old_prefix_score;
    lm_state* state = new lm_state(h, id);
    state->calculate_prefix_suffix(lm_order);
    state->set_score(prefix_score);
    lm_feature->add_state(state);
    score += prefix_score;

    return score;
}

static float backward_lm_feature_function(const hypothesis* h, unsigned int id)
{
    bool calc = false;
    bool prefix = true;
    float full = 0.0f;
    float score = 0.0f;
    float prev_score = 0.0f;
    float prefix_score = 0.0f;
    const rule* r = h->get_rule();
    auto& rule_body = r->get_target_rule_body();
    unsigned int rule_body_size = rule_body.size();
    configuration* config = configuration::get_instance();
    model* system_model = config->get_model();
    language_model* lm = system_model->get_language_model(id);
    ngram_language_model* nlm = static_cast<ngram_language_model*>(lm);
    feature* lm_feature = const_cast<feature*>(h->get_feature(id));
    float old_prefix_score = 0.0f;
    unsigned int lm_order = nlm->get_order();
    unsigned int indx = h->get_previous_hypothesis_number() - 1;
    std::vector<const std::string*> ngram;

    // traverse rule body in reversed order
    // note in backward language model mode,
    // prefix and suffix's role is reversed,
    // and must be used in reversed order
    for (unsigned int i = 0; i < rule_body_size; i++) {
        const symbol* sym = rule_body[rule_body_size - 1 - i];

        if (sym->is_terminal()) {
            // <s> and </s> should be reversed
            ngram.push_back(sym->get_name());
        } else {
            hypothesis* prev_hypo = h->get_previous_hypothesis(indx--);
            unsigned int prev_term_num = prev_hypo->get_terminal_number();
            const feature* prev_fea = prev_hypo->get_feature(id);
            state* prev_state = prev_fea->get_state();
            lm_state* prev_lm_state = static_cast<lm_state*>(prev_state);
            auto hypo_prefix = prev_lm_state->get_prefix();
            auto hypo_suffix = prev_lm_state->get_suffix();
            // suffix is actually prefix here
            auto iter_begin = hypo_suffix->rbegin();
            auto iter_end = hypo_suffix->rend();

            prev_score += prev_fea->get_score();
            old_prefix_score += prev_lm_state->get_score();
            ngram.insert(ngram.end(), iter_begin, iter_end);
            transform(ngram);

            if (prev_term_num >= lm_order - 1) {
                calc = true;
            }

            if (calc) {
                // prefix is actually suffix here
                iter_begin = hypo_prefix->rbegin();
                iter_end = hypo_prefix->rend();

                if (i == 0 && prev_term_num >= lm_order - 1) {
                    prefix_score = prev_lm_state->get_score();
                } else {
                    float ngram_score = 0.0;
                    float full_score = 0.0;
                    nlm->probability(ngram, full_score, ngram_score);

                    if (prefix)
                        prefix_score = full_score - ngram_score;
                    full += full_score;
                    score += ngram_score;
                }

                ngram.clear();
                ngram.insert(ngram.end(), iter_begin, iter_end);
                transform(ngram);
                calc = false;
                prefix = false;
            }
        }
    }

    float full_score = 0.0;
    float ngram_score = 0.0;
    nlm->probability(ngram, full_score, ngram_score);

    if (prefix)
        prefix_score = full_score - ngram_score;

    score += ngram_score;
    full += full_score;
    score += prev_score;
    score -= old_prefix_score;
    lm_state* state = new lm_state(h, id);
    state->calculate_prefix_suffix(lm_order);
    state->set_score(prefix_score);
    lm_feature->add_state(state);
    score += prefix_score;

    return score;
}

float joint_model_feature_function(const hypothesis* hypo, unsigned int id)
{
    configuration* config = configuration::get_instance();
    model* system_model = config->get_model();
    language_model* lm = system_model->get_language_model(id);
    language_model_type type = lm->get_type();

    if (type == language_model_type::forward)
        return forward_jm_feature_function(hypo, id);
    else if (type == language_model_type::backward)
        return backward_jm_feature_function(hypo, id);
    else
        return 0.0f;
}
//#include <iostream>
float lexical_model_feature_function(const hypothesis* hypo, unsigned int id)
{
    configuration* config = configuration::get_instance();
    model* system_model = config->get_model();
    language_model* lm = system_model->get_language_model(id);
    lexical_model* lex = static_cast<lexical_model*>(lm);
    const partial_rule* srule = hypo->get_partial_rule();
    const rule* trule = hypo->get_rule();
    information* info = hypo->get_input();
    auto& rule_body = trule->get_target_rule_body();
    unsigned int rule_body_size = rule_body.size();
    const alignment* align = trule->get_alignment();
    const std::vector<const symbol*>& source = *info->get_sentence();
    std::vector<const symbol*> source_rule_body;
    unsigned int source_size = source.size();
    unsigned int source_rule_size;
    unsigned int window_size = lex->get_window_size();
    unsigned int rwin = window_size / 2;
    unsigned int lwin = window_size - rwin - 1;
    auto span = hypo->get_span();
    float score = 0.0;

    get_source_rule(srule, source_rule_body);
    source_rule_size = source_rule_body.size();

    // get source alignment
    std::vector<std::vector<unsigned int>> src_align_vec;
    src_align_vec.resize(source_rule_size);
    auto& full_align = align->get_alignment();
    unsigned int full_align_size = full_align.size();

    for (unsigned int i = 0; i < full_align_size; i++) {
        auto& align_pair = full_align[i];
        src_align_vec[align_pair.first].push_back(align_pair.second);
    }

    // get absolute position of source symbol
    std::vector<unsigned int> pos_vec(source_rule_size, 0);
    pos_vec[0] = span.first;

    for (unsigned int i = 0; i < rule_body_size; i++) {
        const symbol* sym = rule_body[i];

        if (sym->is_nonterminal()) {
            unsigned int tntid = align->get_target_nonterminal_index(i);
            hypothesis* prev_hypo = hypo->get_previous_hypothesis(tntid);
            auto pspan = prev_hypo->get_span();
            unsigned int sntid;
            unsigned int index;

            sntid = align->get_nonterminal_map(tntid, direction::target);
            index = align->get_source_nonterminal_position(sntid);

            pos_vec[index] = pspan.second;
        }
    }

    for (unsigned int i = 0; i < source_rule_size; i++) {
        if (i && pos_vec[i] == 0) {
            pos_vec[i] = pos_vec[i - 1] + 1;
        }
    }

    std::vector<const std::string*> input;
    symbol_table* symtab = symbol_table::get_instance();
    const symbol* bos = symtab->search_symbol("<s>");
    const symbol* eos = symtab->search_symbol("</s>");
    const symbol* null = symtab->search_symbol("<null>");
    input.reserve(window_size + 1);

    for (unsigned int i = 0; i < source_rule_size; i++) {
        const symbol* sym = source_rule_body[i];
        unsigned int pos = pos_vec[i];
        auto& align_vec = src_align_vec[i];
        unsigned int vec_size = align_vec.size();

        if (sym->is_nonterminal()) {
            // get previous score
            unsigned int sntid = align->get_source_nonterminal_index(i);
            hypothesis* prev_hypo = hypo->get_previous_hypothesis(sntid);
            const feature* prev_fea = prev_hypo->get_feature(id);

            score += prev_fea->get_score();
            continue;
        }

        for (unsigned int j = 0; j < lwin; j++) {
            int index = pos - lwin + j;

            if (index < 0)
                input.push_back(bos->get_name());
            else
                input.push_back(source[index]->get_name());
        }

        input.push_back(source[pos]->get_name());

        for (unsigned int j = 0; j < rwin; j++) {
            int index = pos + 1 + j;

            if (index >= static_cast<int>(source_size))
                input.push_back(eos->get_name());
            else
                input.push_back(source[index]->get_name());
        }

        if (vec_size == 0) {
            input.push_back(null->get_name());
        } else if (vec_size == 1) {
            unsigned int tgt_pos = align_vec[0];
            const symbol* target_symbol = rule_body[tgt_pos];

            if (target_symbol->is_nonterminal()) {
                unsigned int tid;
                const hypothesis* h;
                const feature* fea;

                tid = align->get_target_nonterminal_index(tgt_pos);
                h = hypo->get_previous_hypothesis(tid);
                fea = h->get_feature(id);
                score += fea->get_score();

                input.clear();
                continue;
            } else {
                input.push_back(target_symbol->get_name());
            }
        } else {
            std::string word;
            std::vector<std::string> str_vec;
            str_vec.reserve(vec_size);
            const symbol* sym;

            for (unsigned int j = 0; j < vec_size; j++) {
                str_vec.push_back(*rule_body[align_vec[j]]->get_name());
            }

            string_join(str_vec, "***", word);
            sym = symtab->search_symbol(word);
            input.push_back(sym->get_name());
        }

        score += lex->probability(input.data());
        input.clear();
    }
/*
    for (unsigned int i = 0; i < source_rule_size; i++)
        std::cout << *source_rule_body[i]->get_name() << " ";
    std::cout << score << std::endl;*/

    return score;
}

float language_model_feature_function(const hypothesis* hypo, unsigned int id)
{
    configuration* config = configuration::get_instance();
    model* system_model = config->get_model();
    language_model* lm = system_model->get_language_model(id);
    language_model_type type = lm->get_type();

    if (type == language_model_type::forward)
        return forward_lm_feature_function(hypo, id);
    else if (type == language_model_type::backward)
        return backward_lm_feature_function(hypo, id);
    else
        return 0.0f;
}

float translation_model_feature_function(const hypothesis* h, unsigned int id)
{
    float score = 0.0f;
    const rule* r = h->get_rule();
    unsigned int size = h->get_previous_hypothesis_number();
    configuration* config = configuration::get_instance();
    model* system_model = config->get_model();
    unsigned int tm_id = r->get_id();
    translation_model* tm = system_model->get_translation_model(tm_id);
    const unsigned int not_found = static_cast<unsigned int> (-1);
    unsigned index = tm->get_index(id);

    if (index != not_found)
        score = r->get_score(index);

    if (!size)
        return score;

    /* sum score from previous hypotheses */
    for (unsigned int i = 0; i < size; i++) {
        hypothesis* hypo = h->get_previous_hypothesis(i);
        const feature* f = hypo->get_feature(id);

        score += f->get_score();
    }

    return score;
}

float word_penalty_feature_function(const hypothesis* hypo, unsigned int id)
{
    configuration* config = configuration::get_instance();
    model* system_model = config->get_model();
    word_penalty_model* wm = system_model->get_word_penalty_model();
    unsigned int terminal_num = hypo->get_terminal_number();
    float penalty = wm->get_penalty();

    return terminal_num * penalty;
}

float unknow_word_feature_function(const hypothesis* hypo, unsigned int id)
{
    float score = 0.0f;
    const float penalty = -100.0f;
    const rule* r = hypo->get_rule();
    unsigned int size = hypo->get_previous_hypothesis_number();
    configuration* config = configuration::get_instance();
    model* system_model = config->get_model();
    unsigned int tm_id = r->get_id();
    unsigned int tm_num = system_model->get_translation_model_number();

    if (tm_id == tm_num - 1)
        score += penalty;

    if (!size)
        return score;

    /* sum score from previous hypotheses */
    for (unsigned int i = 0; i < size; i++) {
        hypothesis* h = hypo->get_previous_hypothesis(i);
        const feature* f = h->get_feature(id);

        score += f->get_score();
    }

    return score;
}
