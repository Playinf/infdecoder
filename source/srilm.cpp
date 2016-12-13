/* srilm.cpp */
#include <vector>
#include <string>
#include <srilm.h>
#include <Ngram.h>
#include <Vocab.h>

inline float floor_score(float score)
{
    const float min_score = -100.0f;

    return std::max(score, min_score);
}

srilm::srilm()
{
    Vocab* vocab = new Vocab();
    Ngram* ngram = new Ngram((*vocab));

    ngram->skipOOVs() = false;

    this->lm = ngram;
    this->vocab = vocab;
}

srilm::srilm(unsigned int order)
{
    Vocab* vocab = new Vocab();
    Ngram* ngram = new Ngram((*vocab), order);

    ngram->skipOOVs() = false;

    this->lm = ngram;
    this->vocab = vocab;
}

srilm::~srilm()
{
    delete lm;
    delete vocab;
}

unsigned int srilm::get_order() const
{
    return order;
}

void srilm::load(const char* filename)
{
    File file(filename, "r");

    lm->read(file);
}

void srilm::set_order(unsigned int order)
{
    this->order = order;
    lm->setorder(order);
}

float srilm::probability(const std::string& word)
{
    // ln(10)
    //const float factor = 2.302585093f;
    std::string* w = const_cast<std::string*>(&word);

    if (word == "<s>")
        return 0.0f;

    float score = word_prob(w, nullptr, 0);

    return floor_score(score);
}

float srilm::probability(std::vector<const std::string*>& ngram)
{
    // ln(10)
    //const float factor = 2.302585093f;
    unsigned int size = ngram.size();
    std::string* word = const_cast<std::string*>(ngram[size - 1]);
    std::string** ctx = const_cast<std::string**>(ngram.data());
    float score;

    score = word_prob(word, ctx, size - 1);

    return floor_score(score);
}

void srilm::probability(const input_type& str, float& full, float& ngram)
{
    std::vector<std::string*> context;
    unsigned int offset = 0;
    unsigned int n = str.size();
    unsigned int context_len = 0;
    // ln(10)
    //const float factor = 2.302585093f;

    full = 0.0f;
    ngram = 0.0f;

    for (unsigned int i = 0; i < n; i++) {
        std::string* word = const_cast<std::string*>(str[i]);
        float prob;

        if (i == 0 && *word == "<s>") {
            context.push_back(word);
            context_len += 1;
            continue;
        }

        std::string** context_ptr = context.data() + offset;

        prob = word_prob(word, context_ptr, context_len);
        prob = floor_score(prob);

        if (context_len >= order - 1) {
            ngram += prob;
            offset += 1;
            context_len -= 1;
        }
        full += prob;
        context.push_back(word);
        context_len += 1;
    }

    //full *= factor;
    //ngram *= factor;
}

float srilm::sentence_prob(std::vector<std::string>& sentence)
{
    float score = 0.0f;
    unsigned int size = sentence.size();
    VocabString* sen = new VocabString[size + 1];
    TextStats stats;

    for (unsigned int i = 0; i < size; i++)
        sen[i] = sentence[i].c_str();

    sen[size] = 0;

    score = lm->sentenceProb(sen, stats);

    delete[] sen;

    return score;
}

float srilm::word_prob(std::vector<std::string>& context, void** state)
{
    float score = 0.0f;
    unsigned int size = context.size();
    unsigned int unknow_id = vocab->unkIndex();
    VocabIndex ngram[size + 1];
    unsigned int lm_id;

    if (size == 0) {
        if (state != nullptr)
            *state = nullptr;
        return score;
    }

    for (unsigned int i = 0; i < size - 1; i++) {
        std::string& word = context[size - 2 - i];
        VocabIndex index = vocab->getIndex(word.c_str(), unknow_id);
        ngram[i + 1] = index;
    }

    ngram[size] = Vocab_None;
    lm_id = vocab->getIndex(context[size - 1].c_str());
    score = lm->wordProb(lm_id, ngram + 1);

    if (state != nullptr) {
        unsigned int len;
        ngram[0] = lm_id;
        *state = lm->contextID(ngram, len);
    }

    return score;
}

float srilm::word_prob(std::string* word, std::string** ctx, int len)
{
    float score = 0.0f;
    VocabString* sen = new VocabString[len + 1];
    LM* ngram = lm;

    for (int i = 0; i < len; i++)
        sen[len - 1 - i] = ctx[i]->c_str();

    sen[len] = 0;

    score = ngram->wordProb(word->c_str(), sen);

    delete[] sen;

    return score;
}
