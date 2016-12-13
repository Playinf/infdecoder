/*
 * joint_model.h
 *
 * author: Playinf
 * email: playinf@stu.xmu.edu.cn
 *
 */
#ifndef __JOINT_MODEL_H__
#define __JOINT_MODEL_H__

#include <language_model.h>

enum class joint_model_type {
    source_to_target, target_to_source
};

class joint_model : public language_model {
public:
    joint_model();
    virtual ~joint_model();

    joint_model(const joint_model&) = delete;
    joint_model& operator=(const joint_model&) = delete;

    joint_model_type get_model_type() const;
    void set_model_type(joint_model_type m);

    virtual unsigned int get_source_context() const = 0;
    virtual unsigned int get_target_context() const = 0;

    virtual void set_source_context(unsigned int n) = 0;
    virtual void set_target_context(unsigned int n) = 0;

    virtual float probability(const std::string** input) = 0;
private:
    joint_model_type model_type;
};

#endif /* __JOINT_MODEL_H__ */
