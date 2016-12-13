/*
 * innltm.h
 *
 * author: Playinf
 * email: playinf@stu.xmu.edu.cn
 *
 */
#ifndef __INNLTM_H__
#define __INNLTM_H__

#include <string>
#include <vector>
#include <lexical_model.h>

#define override ;

namespace infinity {
namespace lm {

class vocab;
class nnltm;

} /* lm */
} /* infinity */

class innltm : public lexical_model {
public:
    innltm();
    ~innltm();

    innltm(const innltm&) = delete;
    innltm& operator=(const innltm&) = delete;

    unsigned int get_window_size() const override;

    void set_window_size(unsigned int n) override;
    void load(const char* filename) override;
    float probability(const std::string** input) override;
private:
    unsigned int order;
    unsigned int window_size;
    infinity::lm::nnltm* model;
    infinity::lm::vocab* input_vocab;
    infinity::lm::vocab* output_vocab;
};

#endif /* __INNLTM_H__ */
