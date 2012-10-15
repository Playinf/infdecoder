/* lm.cpp */
#include <lm.h>

void* lm::model;
unsigned int lm::order;

void lm::ngram_probability(const std::vector<const std::string*>& str,
        double& full, double& ngram)
{
    std::string context;
    unsigned int offset = 0;
    unsigned int n = str.size();
    unsigned int context_len = 0;

    for (unsigned int i = 0; i < n; i++) {
        const std::string& word = *str[i];
        double prob;

        if (i == 0 && word == "<s>") {
            context += "<s> ";
            context_len += 1;
            continue;
        }

        const char* context_ptr = context.c_str() + offset;
        prob = sriWordProb(model, word.c_str(), context_ptr);

        if (context_len >= order - 1) {
            ngram += prob;
            offset += str[i - order + 1]->length() + 1;
        }
        full += prob;
        context += word;
        context += " ";
        context_len += 1;
    }
}

double lm::word_probability(const std::string& word)
{
    if (word == "<s>")
        return 0.0;

    return sriWordProb(model, word.c_str(), "");
}
