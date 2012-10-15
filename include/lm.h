/*
 * lm.h
 * implementation of language model
 *
 * author: Playinf
 * playinf@stu.xmu.edu.cn
 *
 */
#ifndef __LM_H__
#define __LM_H__

#include <string>
#include <vector>

#ifdef _WIN32
#include <lmsridll.h>
#else
#include <lmsri.h>
#endif

class lm {
public:

    static void load(const char* file)
    {
        model = sriLoadLM(file, 1, order, 1, 0);
    }

    static void unload()
    {
        sriUnloadLM(model);
    }

    static void set_language_model_order(unsigned int n)
    {
        order = n;
    }

    static unsigned int get_language_model_order()
    {
        return order;
    }

    static void ngram_probability(const std::vector<const std::string*>& str,
        double& full, double& ngram);
    static double word_probability(const std::string& word);

private:
    static unsigned int order;
    static void* model;
};

#endif /* __LM_H__ */
