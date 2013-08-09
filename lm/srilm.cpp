/* srilm.cpp */
#include <vector>
#include <string>
#include <srilm.h>
#include <Ngram.h>
#include <Vocab.h>

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

void srilm::load(const char* filename)
{
    File file(filename, "r");

    lm->read(file);
}

void srilm::set_order(unsigned int order)
{
    lm->setorder(order);
}

float srilm::sentence_probability(std::vector<std::string>& sentence)
{
    float score = 0.0f;
    unsigned int size = sentence.size();
    VocabString* sen = new VocabString[size + 1];
    LM* ngram = lm;
    TextStats stats;

    for (unsigned int i = 0; i < size; i++)
        sen[i] = sentence[i].c_str();

    sen[size] = 0;

    score = ngram->sentenceProb(sen, stats);

    delete[] sen;

    return score;
}

float srilm::word_probability(std::vector<std::string>& context, void** state)
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

float srilm::word_probability(std::string* word, std::string** ctx, int len)
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
