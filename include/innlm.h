/*
 * innlm.h
 * feed forward neural network language model
 *
 * author: Playinf
 * email: playinf@stu.xmu.edu.cn
 *
 */
#ifndef __INNLM_H__
#define __INNLM_H__

#include <string>
#include <vector>
#include <ngram_language_model.h>

#define override ;

namespace infinity {
namespace lm {

class nnlm;
class vocab;

} /* lm */
} /* infinity */

class innlm : public ngram_language_model {
public:
    innlm();
    ~innlm();

    innlm(const innlm&) = delete;
    innlm& operator=(const innlm&) = delete;

    unsigned int get_order() const override;

    void set_order(unsigned int n) override;
    void load(const char* filename) override;
    float probability(const std::string& word) override;
    float probability(std::vector<const std::string*>& ngram) override;
    void probability(const input_type& in, float& fs, float& ns) override;
private:
    float ngram_prob(const std::string** ngram, unsigned int len);
private:
    unsigned int order;
    infinity::lm::nnlm* model;
    infinity::lm::vocab* input_vocab;
    infinity::lm::vocab* output_vocab;
};

#endif /* __INNLM_H__ */
