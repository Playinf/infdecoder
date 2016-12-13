/*
 * ngram_language_model.h
 *
 * author: Playinf
 * email: playinf@stu.xmu.edu.cn
 *
 */
#ifndef __NGRAM_LANGUAGE_MODEL_H__
#define __NGRAM_LANGUAGE_MODEL_H__

#include <language_model.h>

class ngram_language_model : public language_model {
public:
    typedef std::vector<const std::string*> input_type;

    ngram_language_model();
    virtual ~ngram_language_model();

    ngram_language_model(const ngram_language_model&) = delete;
    ngram_language_model& operator=(const ngram_language_model&) = delete;

    virtual unsigned int get_order() const = 0;

    virtual void set_order(unsigned int n) = 0;

    virtual void load(const char* filename) = 0;
    virtual float probability(const std::string& s) = 0;
    virtual float probability(input_type& n) = 0;
    virtual void probability(const input_type& in, float& fs, float& ns) = 0;
};

#endif /* __NGRAM_LANGUAGE_MODEL_H__ */
