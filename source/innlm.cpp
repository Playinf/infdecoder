/* innlm.cpp */
#include <string>
#include <vector>
#include <nnlm.h>
#include <innlm.h>
#include <vocab.h>
#include <symbol.h>
#include <language_model.h>

inline float floor_score(float score)
{
    const float min_score = -100.0f;

    return std::max(score, min_score);
}

innlm::innlm()
{
    order = 0;
    model = nullptr;
    input_vocab = nullptr;
    output_vocab = nullptr;
}

innlm::~innlm()
{
    if (model != nullptr)
        delete model;
}

unsigned int innlm::get_order() const
{
    return order;
}

void innlm::set_order(unsigned int n)
{
    order = n;
}

void innlm::load(const char* name)
{
    if (model != nullptr) {
        delete model;
    }
    model = new infinity::lm::nnlm;
    model->load(name);
    input_vocab = model->get_input_vocab();
    output_vocab = model->get_output_vocab();
}

float innlm::probability(const std::string& word)
{
    const std::string* w = &word;

    if (word == "<s>")
        return 0.0f;

    float score = ngram_prob(&w, 1);

    return floor_score(score);
}

float innlm::probability(std::vector<const std::string*>& ngram)
{
    float score;

    score = ngram_prob(ngram.data(), ngram.size());

    return floor_score(score);
}

void innlm::probability(const input_type& in, float& fs, float& ns)
{
    unsigned int offset = 0;
    unsigned int n = in.size();
    unsigned int context_len = 0;
    auto& str = const_cast<std::vector<const std::string*>&>(in);

    fs = 0.0f;
    ns = 0.0f;

    for (unsigned int i = 0; i < n; i++) {
        std::string* word = const_cast<std::string*>(str[i]);
        float prob;

        if (i == 0 && *word == "<s>") {
            context_len += 1;
            continue;
        }

        const std::string** ngram_ptr = str.data() + offset;

        prob = ngram_prob(ngram_ptr, context_len + 1);
        prob = floor_score(prob);

        if (context_len >= order - 1) {
            ns += prob;
            offset += 1;
            context_len -= 1;
        }
        fs += prob;
        context_len += 1;
    }
}

float innlm::ngram_prob(const std::string** ngram, unsigned int len)
{
    double score = 0.0;
    unsigned int* input;
    unsigned int size;
    unsigned int iunk_id;
    unsigned int ounk_id;
    unsigned int index = 0;

    iunk_id = input_vocab->get_id("<unk>");
    ounk_id = output_vocab->get_id("<unk>");

    if (*ngram[0] == "<s>") {
        if (len == 1)
            return 0.0;

        // padding <s>
        size = len + order - 2;
        input = new unsigned int[size];
        unsigned int bos_id = input_vocab->get_id("<s>");

        for (unsigned int i = 0; i < order - 1; i++)
            input[i] = bos_id;
        index = order;
    } else if (len < order) {
        index = order - len;

        // padding <null>
        size = order;
        input = new unsigned int[size];
        unsigned int null_id = input_vocab->get_id("<null>");

        for (unsigned int i = 0; i < index; i++) {
            input[i] = null_id;
        }
    } else {
        size = len;
        input = new unsigned int[size];
    }

    for (unsigned int i = 0; i < len; i++) {
        auto iter = input_vocab->find(*ngram[i]);

        if (iter != input_vocab->end())
            input[index + i] = iter->second;
        else
            input[index + i] = iunk_id;
    }

    for (unsigned int i = 0; i < size; i++) {
        if (i + order - 1 >= size)
            break;
        unsigned int label = ounk_id;
        auto iter = output_vocab->find(*ngram[i + order - 1]);

        if (iter != output_vocab->end())
            label = iter->second;

        score += model->ngram_prob(input + i, label);
    }

    delete[] input;

    return score;
}
