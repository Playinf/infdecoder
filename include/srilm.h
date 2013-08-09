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

class Ngram;
class Vocab;

class srilm {
public:
    srilm();
    srilm(unsigned int order);
    ~srilm();

    srilm(const srilm& lm) = delete;
    srilm& operator=(const srilm& lm) = delete;

    void load(const char* filename);
    void set_order(unsigned int order);
    float sentence_probability(std::vector<std::string>& sentence);
    float word_probability(std::vector<std::string>& context, void **state);
    float word_probability(std::string* word, std::string** context, int len);
private:
    Ngram* lm;
    Vocab* vocab;
};

#endif /* __SRILM_H__ */
