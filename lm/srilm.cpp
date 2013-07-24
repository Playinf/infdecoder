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

    this->lm = static_cast<void*>(ngram);
    this->vocab = static_cast<void*>(vocab);
}

srilm::~srilm()
{
    Ngram* ngram = static_cast<Ngram*>(this->lm);
    Vocab* vocab = static_cast<Vocab*>(this->vocab);

    delete ngram;
    delete vocab;
}

void srilm::load(const char* filename)
{
    File file(filename, "r");
    LM* ngram = static_cast<Ngram*>(lm);

    ngram->read(file);
}

void srilm::set_order(unsigned int order)
{
    Ngram* ngram = static_cast<Ngram*>(lm);

    ngram->setorder(order);
}

float srilm::sentence_probability(std::vector<std::string>& sentence)
{
    float score = 0.0f;
    unsigned int size = sentence.size();
    VocabString* sen = new VocabString[size + 1];
    LM* ngram = static_cast<Ngram*>(lm);
    TextStats stats;

    for (unsigned int i = 0; i < size; i++)
        sen[i] = sentence[i].c_str();

    sen[size] = 0;

    score = ngram->sentenceProb(sen, stats);

    delete[] sen;

    return score;
}

float srilm::word_probability(std::string* word, std::string** ctx, int len)
{
    float score = 0.0f;
    VocabString* sen = new VocabString[len + 1];
    LM* ngram = static_cast<Ngram*>(lm);
    TextStats stats;

    for (unsigned int i = 0; i < len; i++)
        sen[i] = ctx[i]->c_str();

    sen[len] = 0;

    score = ngram->wordProb(word->c_str(), sen);

    delete[] sen;

    return score;
}
