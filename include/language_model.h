/*
 * language_model.h
 *
 *
 * author: Playinf
 * email: playinf@stu.xmu.edu.cn
 *
 */
#ifndef __LM_H__
#define __LM_H__

#include <string>
#include <vector>
#include <lmsri.h>

class language_model {
public:
    typedef std::vector<const std::string*> input_type;

    language_model();
    language_model(const char* filename);
    ~language_model();

    language_model(const language_model& lm) = delete;
    language_model& operator=(const language_model* lm) = delete;

    void load(const char* filename);
    void unload();
    void set_order(unsigned int n);
    unsigned int get_order() const;
    void ngram_probability(const input_type& in, float& s, float& n) const;
    float word_probability(const std::string& word) const;
private:
    unsigned int order;
    void* model;
};

#endif /* __LM_H__ */
