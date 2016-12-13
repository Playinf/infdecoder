/*
 * lexical_model.h
 *
 * author: Playinf
 * email: playinf@stu.xmu.edu.cn
 *
 */
#ifndef __LEXICAL_MODEL_H__
#define __LEXICAL_MODEL_H__

#include <string>
#include <language_model.h>

enum class lexical_model_type {
    source_to_target, target_to_source
};

class lexical_model : public language_model {
public:
    lexical_model();
    virtual ~lexical_model();

    lexical_model(const lexical_model&) = delete;
    lexical_model& operator=(const lexical_model&) = delete;

    lexical_model_type get_model_type() const;
    virtual unsigned int get_window_size() const = 0;

    void set_model_type(lexical_model_type type);
    virtual void set_window_size(unsigned int n) = 0;
    virtual float probability(const std::string** input) = 0;
private:
    lexical_model_type model_type;
};

#endif /* __LEXICAL_MODEL_H__ */
