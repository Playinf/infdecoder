/*
 * language_model.h
 *
 *
 * author: Playinf
 * email: playinf@stu.xmu.edu.cn
 *
 */
#ifndef __LANGUAGE_MODEL_H__
#define __LANGUAGE_MODEL_H__

#include <mutex>
#include <string>
#include <vector>

enum class language_model_type {
    none, forward, backward
};

class language_model {
public:
    language_model();
    virtual ~language_model();

    language_model(const language_model&) = delete;
    language_model& operator=(const language_model&) = delete;

    unsigned int get_id() const;
    unsigned int get_feature_id() const;
    language_model_type get_type() const;

    void set_id(unsigned int id);
    void set_feature(unsigned int id);
    void set_type(language_model_type type);

    virtual void load(const char* filename) = 0;
private:
    unsigned int id;
    unsigned int feature_id;
    language_model_type type;
};

#endif /* __LANGUAGE_MODEL_H__ */
