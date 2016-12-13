/*
 * innjm.h
 * feed forward neural network joint model
 *
 * author: Playinf
 * email: playinf@stu.xmu.edu.cn
 */
 #ifndef __INNJM_H__
 #define __INNJM_H__

#include <string>
#include <vector>
#include <joint_model.h>

#define override ;

namespace infinity {
namespace lm {

class nnjm;
class vocab;

} /* lm */
} /* infinity */

class innjm : public joint_model {
public:
    innjm();
    ~innjm();

    innjm(const innjm&) = delete;
    innjm& operator=(const innjm&) = delete;

    unsigned int get_source_context() const override;
    unsigned int get_target_context() const override;

    void load(const char* filename) override;
    void set_source_context(unsigned int n) override;
    void set_target_context(unsigned int n) override;
    float probability(const std::string** input) override;
private:
    unsigned int context[2];
    infinity::lm::nnjm* model;
    infinity::lm::vocab* source_vocab;
    infinity::lm::vocab* target_vocab;
    infinity::lm::vocab* output_vocab;
};

 #endif /* __INNJM_H__ */
