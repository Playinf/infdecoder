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

#include <mutex>
#include <string>
#include <vector>

class srilm;

enum language_model_type {
    SRI
};

class language_model {
public:
    typedef std::vector<const std::string*> input_type;

    language_model();
    language_model(const char* filename);
    ~language_model();

    language_model(const language_model& lm) = delete;
    language_model& operator=(const language_model* lm) = delete;

    unsigned int get_id() const;
    unsigned int get_order() const;
    unsigned int get_feature_id() const;
    language_model_type get_type() const;

    void load(const char* filename);
    void set_id(unsigned int id);
    void set_order(unsigned int n);
    void set_feature(unsigned int id);
    void set_type(language_model_type type);
    void ngram_probability(const input_type& in, float& s, float& n);
    float word_probability(const std::string& word);
    float word_probability(std::vector<const std::string*>& ngram);
private:
    srilm* model;
    unsigned int id;
    unsigned int order;
    unsigned int feature_id;
    language_model_type type;
};

#endif /* __LM_H__ */
