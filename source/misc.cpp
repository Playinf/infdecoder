/* misc.cpp */
#include <string>
#include <vector>
#include <utility>
#include <rule.h>
#include <symbol.h>
#include <trellis.h>
#include <alignment.h>
#include <chart_cell.h>
#include <hypothesis.h>
#include <partial_rule.h>

typedef std::pair<unsigned int, unsigned int> align_type;

static void output_alignment(const hypothesis* hypo, unsigned int begin,
    std::vector<align_type>& align)
{
    const rule* r = hypo->get_rule();
    auto& body = r->get_target_rule_body();
    const alignment* ralign = r->get_alignment();
    auto& span = hypo->get_span();

    // no alignment information
    if (ralign == nullptr)
        return;

    std::vector<unsigned int> pos_vec;
    unsigned int size = body.size();
    unsigned int pos = begin;
    unsigned int nonterm = 0;

    for (unsigned int i = 0; i < size; i++) {
        const symbol* sym = body[i];

        if (sym->is_terminal()) {
            pos_vec.push_back(pos);
            pos += 1;
        } else {
            const hypothesis* phypo = hypo->get_previous_hypothesis(nonterm++);
            pos_vec.push_back(pos);
            pos += phypo->get_terminal_number();
        }
    }

    auto& align_vec = ralign->get_alignment();
    unsigned int src_begin = span.first;
    size = align_vec.size();
    nonterm = 0;

    for (unsigned int i = 0; i < size; i++) {
        auto& align_pair = align_vec[i];
        unsigned int tpos = align_pair.second;
        const symbol* tgt_symbol = body[tpos];

        if (tgt_symbol->is_terminal()) {
            unsigned int src = src_begin + align_pair.first;
            unsigned int tgt = pos_vec[i];
            align.push_back(std::make_pair(src, tgt));
        } else {
            unsigned index = ralign->get_nonterminal_map(nonterm++);
            const hypothesis* phypo = hypo->get_previous_hypothesis(index);
            auto& pspan = phypo->get_span();
            output_alignment(phypo, pos_vec[tpos], align);
            src_begin = pspan.second - align_pair.first;
        }
    }
}

static void output_alignment(const trellis_node* node, unsigned int begin,
    std::vector<align_type>& align)
{
    const hypothesis* hypo = node->get_hypothesis();
    const rule* r = hypo->get_rule();
    auto& body = r->get_target_rule_body();
    auto& span = hypo->get_span();
    const alignment* ralign = r->get_alignment();

    // no alignment information
    if (ralign == nullptr)
        return;

    std::vector<unsigned int> pos_vec;
    unsigned int size = body.size();
    unsigned int pos = begin;
    unsigned int nonterm = 0;

    for (unsigned int i = 0; i < size; i++) {
        const symbol* sym = body[i];

        if (sym->is_terminal()) {
            pos_vec.push_back(pos);
            pos += 1;
        } else {
            const trellis_node* pnode = node->get_child(nonterm++);
            const hypothesis* phypo = pnode->get_hypothesis();
            pos_vec.push_back(pos);
            pos += phypo->get_terminal_number();
        }
    }

    auto& align_vec = ralign->get_alignment();
    unsigned int src_begin = span.first;
    size = align_vec.size();
    nonterm = 0;

    for (unsigned int i = 0; i < size; i++) {
        auto& align_pair = align_vec[i];
        unsigned int tpos = align_pair.second;
        const symbol* tgt_symbol = body[tpos];

        if (tgt_symbol->is_terminal()) {
            unsigned int src = src_begin + align_pair.first;
            unsigned int tgt = pos_vec[i];
            align.push_back(std::make_pair(src, tgt));
        } else {
            unsigned index = ralign->get_nonterminal_map(nonterm++);
            const trellis_node* pnode = node->get_child(index);
            const hypothesis* phypo = pnode->get_hypothesis();
            auto& pspan = phypo->get_span();
            output_alignment(phypo, pos_vec[tpos], align);
            src_begin = pspan.second - align_pair.first;
        }
    }
}

void output_full_alignment(const hypothesis* h, std::vector<align_type>& align)
{
    output_alignment(h, 0, align);
}

void output_full_alignment(const trellis_path* p, std::vector<align_type>& a)
{
    output_alignment(p->get_final_node(), 0, a);
}
