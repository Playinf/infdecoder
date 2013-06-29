/*
 * lm.h
 * implementation of language model
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

class lm {
public:

    lm()
    {
    }

    lm(const char* filename)
    {
        load(filename);
    }

    ~lm
    {
        sriUnloadLM(model);
    }

    void load(const char* filename)
    {
        model = sriLoadLM(filename, 1, order, 1, 0);
    }

    void unload()
    {
        sriUnloadLM(model);
    }

    void set_language_model_order(unsigned int n)
    {
        order = n;
    }

    unsigned int get_language_model_order()
    {
        return order;
    }

    void ngram_probability(const std::vector<const std::string*>& str,
        double& full, double& ngram);
    double word_probability(const std::string& word);

private:
    unsigned int order;
    void* model;
};

#endif /* __LM_H__ */
