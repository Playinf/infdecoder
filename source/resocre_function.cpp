/* rescore_function.cpp */
#include <utility>
#include <stdexcept>
#include <misc.h>
#include <rule.h>
#include <model.h>
#include <config.h>
#include <symbol.h>
#include <feature.h>
#include <utility.h>
#include <alignment.h>
#include <chart_cell.h>
#include <hypothesis.h>
#include <information.h>
#include <joint_model.h>
#include <partial_rule.h>
#include <lexical_model.h>
#include <language_model.h>
#include <rescore_function.h>
#include <ngram_language_model.h>

// defined in standard.cpp
void output_hypothesis(const hypothesis* h, std::vector<const std::string*>& s);

static int get_affiliation(std::vector<std::vector<int>>& vec, int ind)
{
    const std::vector<int>* pos_list = &vec[ind];
    int size = vec.size();

    if (pos_list->size() == 0) {
        int rpos = ind + 1;
        int lpos = ind - 1;

        while (rpos < size || lpos >= 0) {
            if (rpos < size) {
                const std::vector<int>* plist = &vec[rpos];

                if (plist->size() == 0) {
                    continue;
                } else {
                    pos_list = plist;
                    break;
                }
            }

            if (lpos >= 0) {
                const std::vector<int>* plist = &vec[lpos];

                if (plist->size() == 0) {
                    continue;
                } else {
                    pos_list = plist;
                    break;
                }
            }

            rpos += 1;
            lpos -= 1;
        }

        if (pos_list->size() == 0) {
            throw std::runtime_error("no alignment in sentence");
        }
    }

    // round down
    return (*pos_list)[(pos_list->size() - 1) / 2];
}

static float forward_lm_rescore_function(const hypothesis* h, unsigned int id)
{
    configuration* config = configuration::get_instance();
    model* system_model = config->get_model();
    language_model* lm = system_model->get_language_model(id);
    ngram_language_model* nlm = static_cast<ngram_language_model*>(lm);
    std::vector<const std::string*> sentence;

    output_hypothesis(h, sentence);

    return nlm->probability(sentence);
}

static float backward_lm_rescore_function(const hypothesis* h, unsigned int id)
{
    configuration* config = configuration::get_instance();
    model* system_model = config->get_model();
    language_model* lm = system_model->get_language_model(id);
    ngram_language_model* nlm = static_cast<ngram_language_model*>(lm);
    std::vector<const std::string*> sentence;

    output_hypothesis(h, sentence);

    unsigned int start = 1;
    unsigned int end = sentence.size();

    // reverse sentence
    while (start < end) {
        const std::string* tmp = sentence[start];
        sentence[start] = sentence[end];
        sentence[end] = tmp;
        start++;
        end--;
    }

    return nlm->probability(sentence);
}

float joint_model_rescore_function(const hypothesis* h, unsigned int id)
{
    information* info = h->get_input();
    const std::vector<const symbol*>& source = *info->get_sentence();
    std::pair<unsigned int, unsigned int> span = h->get_span();

    // check if is the last chart cell
    if (span.first != 0 || span.second != source.size()) {
        return 0.0f;
    }

    configuration* config = configuration::get_instance();
    model* system_model = config->get_model();
    language_model* lm = system_model->get_language_model(id);
    joint_model* jm = static_cast<joint_model*>(lm);
    language_model_type lm_type;
    joint_model_type jm_type;
    std::vector<const std::string*> target;
    std::vector<std::pair<unsigned int, unsigned int>> full_align;
    std::vector<std::vector<int>> align_vec;
    std::vector<const std::string*> ngram;
    unsigned int source_context = jm->get_source_context();
    unsigned int target_context = jm->get_target_context();

    lm_type = jm->get_type();
    jm_type = jm->get_model_type();
    output_hypothesis(h, target);
    output_full_alignment(h, full_align);
    ngram.reserve(source_context + target_context + 1);

    int ssize = source.size();
    int tsize = target.size();
    int asize = full_align.size();
    const std::string* bos = target[0];
    const std::string* eos = target[tsize - 1];
    int m = source_context;
    int n = target_context;
    unsigned int osize = 0;
    int rctx = 0;
    int lctx = 0;
    float prob = 0.0f;

    if (jm_type == joint_model_type::source_to_target) {
        align_vec.resize(tsize);

        for (int i = 0; i < asize; i++) {
            auto& pair = full_align[i];
            align_vec[pair.second].push_back(pair.first);
        }

        rctx = m / 2;
        lctx = m - rctx - 1;
        osize = tsize;
    } else {
        align_vec.resize(ssize);

        for (int i = 0; i < asize; i++) {
            auto& pair = full_align[i];
            align_vec[pair.first].push_back(pair.second);
        }

        rctx = n / 2;
        lctx = n - rctx - 1;
        osize = ssize;
    }

    // calculate affiliation
    for (unsigned int i = 0; i < osize; i++) {
        int index = get_affiliation(align_vec, i);
        int spos;
        int tpos;

        if (jm_type == joint_model_type::source_to_target) {
            spos = index;
            tpos = i;

            // source context
            for (int j = 0; j < lctx; j++) {
                int pos = spos - lctx + j;

                if (pos < 0)
                    ngram.push_back(bos);
                else
                    ngram.push_back(source[pos]->get_name());
            }

            ngram.push_back(source[spos]->get_name());

            for (int j = 0; j < rctx; j++) {
                int pos = spos + j + 1;

                if (pos >= ssize)
                    ngram.push_back(eos);
                else
                    ngram.push_back(source[pos]->get_name());
            }

            // target context
            for (int j = 0; j < n; j++) {
                int pos;

                if (lm_type == language_model_type::forward) {
                    pos = tpos - n + i;

                    if (pos < 0)
                        ngram.push_back(bos);
                    else
                        ngram.push_back(target[pos]);
                } else {
                    pos = tpos + n - j;

                    if (pos > tsize)
                        ngram.push_back(eos);
                    else
                        ngram.push_back(target[pos]);
                }
            }

            ngram.push_back(target[tpos]);
        } else {
            spos = i;
            tpos = index;

            // source context
            for (int j = 0; j < m; j++) {
                int pos;

                if (lm_type == language_model_type::forward) {
                    pos = spos - m + i;

                    if (pos < 0)
                        ngram.push_back(bos);
                    else
                        ngram.push_back(source[pos]->get_name());
                } else {
                    pos = spos + m - j;

                    if (pos > tsize)
                        ngram.push_back(eos);
                    else
                        ngram.push_back(source[pos]->get_name());
                }
            }

            // target context
            for (int j = 0; j < lctx; j++) {
                int pos = tpos - lctx + j;

                if (pos < 0)
                    ngram.push_back(bos);
                else
                    ngram.push_back(target[pos]);
            }

            ngram.push_back(target[tpos]);

            for (int j = 0; j < rctx; j++) {
                int pos = tpos + j + 1;

                if (pos >= tsize)
                    ngram.push_back(eos);
                else
                    ngram.push_back(target[pos]);
            }

            ngram.push_back(source[spos]->get_name());
        }

        prob += jm->probability(ngram.data());
        ngram.clear();
    }

    return prob;
}

float lexical_model_rescore_function(const hypothesis* h, unsigned int id)
{
    information* info = h->get_input();
    const std::vector<const symbol*>& source = *info->get_sentence();
    std::pair<unsigned int, unsigned int> span = h->get_span();

    // check if is the last chart cell
    if (span.first != 0 || span.second != source.size()) {
        return 0.0f;
    }

    configuration* config = configuration::get_instance();
    model* system_model = config->get_model();
    language_model* lm = system_model->get_language_model(id);
    lexical_model* lex = static_cast<lexical_model*>(lm);
    lexical_model_type type = lex->get_model_type();
    std::vector<const std::string*> target;
    std::vector<std::pair<unsigned int, unsigned int>> full_align;
    std::vector<const std::string*> input;
    unsigned int source_size = source.size();
    unsigned int target_size = target.size();
    unsigned int window_size = lex->get_window_size();
    unsigned int rwin = window_size / 2;
    unsigned int lwin = window_size - rwin - 1;
    unsigned int align_size;
    float score = 0.0f;

    output_hypothesis(h, target);
    output_full_alignment(h, full_align);
    input.reserve(window_size + 1);
    align_size = full_align.size();

    symbol_table* symtab = symbol_table::get_instance();
    const symbol* bos = symtab->search_symbol("<s>");
    const symbol* eos = symtab->search_symbol("</s>");
    const symbol* null = symtab->search_symbol("<null>");

    if (type == lexical_model_type::source_to_target) {
        std::vector<std::vector<int>> align_vec;
        align_vec.resize(source_size);

        for (unsigned int i = 0; i < align_size; i++) {
            auto& pair = full_align[i];
            align_vec[pair.first].push_back(pair.second);
        }

        for (unsigned int i = 0; i < source_size; i++) {
            auto& tgt_align = align_vec[i];
            unsigned int vec_size = tgt_align.size();

            for (unsigned int j = 0; j < lwin; j++) {
                int index = i - lwin + j;

                if (index < 0)
                    input.push_back(bos->get_name());
                else
                    input.push_back(source[index]->get_name());
            }

            input.push_back(source[i]->get_name());

            for (unsigned int j = 0; j < rwin; j++) {
                int index = i + 1 + j;

                if (index >= static_cast<int>(source_size))
                    input.push_back(eos->get_name());
                else
                    input.push_back(source[index]->get_name());
            }

            if (vec_size == 0) {
                input.push_back(null->get_name());
            } else {
                std::string word;
                std::vector<std::string> str_vec;
                str_vec.reserve(vec_size);
                const symbol* sym;

                for (unsigned int j = 0; j < vec_size; j++) {
                    str_vec.push_back(*target[tgt_align[j]]);
                }

                string_join(str_vec, "+++", word);
                sym = symtab->search_symbol(word);
                input.push_back(sym->get_name());
            }

            score += lex->probability(input.data());
            input.clear();
        }
    } else {
        std::vector<std::vector<int>> align_vec;
        align_vec.resize(target_size);

        for (unsigned int i = 0; i < align_size; i++) {
            auto& pair = full_align[i];
            align_vec[pair.second].push_back(pair.first);
        }

        for (unsigned int i = 0; i < target_size; i++) {
            auto& src_align = align_vec[i];
            unsigned int vec_size = src_align.size();

            for (unsigned int j = 0; j < lwin; j++) {
                int index = i - lwin + j;

                if (index < 0)
                    input.push_back(bos->get_name());
                else
                    input.push_back(target[index]);
            }

            input.push_back(target[i]);

            for (unsigned int j = 0; j < rwin; j++) {
                int index = i + 1 + j;

                if (index >= static_cast<int>(target_size))
                    input.push_back(bos->get_name());
                else
                    input.push_back(target[index]);
            }

            if (vec_size == 0) {
                input.push_back(null->get_name());
            } else {
                std::string word;
                std::vector<std::string> str_vec;
                str_vec.reserve(vec_size);
                const symbol* sym;

                for (unsigned int j = 0; j < vec_size; j++) {
                    str_vec.push_back(*source[src_align[j]]->get_name());
                }

                string_join(str_vec, "+++", word);
                sym = symtab->search_symbol(word);
                input.push_back(sym->get_name());
            }

            score += lex->probability(input.data());
            input.clear();
        }
    }

    return score;
}

float language_model_rescore_function(const hypothesis* h, unsigned int id)
{
    information* info = h->get_input();
    const std::vector<const symbol*>& input = *info->get_sentence();
    std::pair<unsigned int, unsigned int> span = h->get_span();

    // check if is the last chart cell
    if (span.first != 0 || span.second != input.size()) {
        return 0.0f;
    }

    configuration* config = configuration::get_instance();
    model* system_model = config->get_model();
    language_model* lm = system_model->get_language_model(id);
    language_model_type type = lm->get_type();

    if (type == language_model_type::forward)
        return forward_lm_rescore_function(h, id);
    else if (type == language_model_type::backward)
        return backward_lm_rescore_function(h, id);
    else
        return 0.0f;
}
