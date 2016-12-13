/*
 * srilm.h
 *
 *
 * author: Playinf
 * email: playinf@stu.xmu.edu.cn
 *
 */
#ifndef __SRILM_H__
#define __SRILM_H__

#include <string>
#include <vector>
#include <ngram_language_model.h>

#define override ;

class Ngram;
class Vocab;

class srilm : public ngram_language_model {
public:
    srilm();
    srilm(unsigned int order);
    ~srilm();

    srilm(const srilm&) = delete;
    srilm& operator=(const srilm&) = delete;

    unsigned int get_order() const override;

    void set_order(unsigned int n) override;
    void load(const char* filename) override;
    float probability(const std::string& word) override;
    float probability(std::vector<const std::string*>& ngram) override;
    void probability(const input_type& in, float& fs, float& ns) override;
private:
    float sentence_prob(std::vector<std::string>& sentence);
    float word_prob(std::vector<std::string>& context, void** state);
    float word_prob(std::string* word, std::string** ctx, int len);

    Ngram* lm;
    Vocab* vocab;
    unsigned int order;
};

#endif /* __SRILM_H__ */
