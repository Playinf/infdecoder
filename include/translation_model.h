/*
 * translation_model.h
 *
 *
 * author: Playinf
 * email: playinf@stu.xmu.edu.cn
 *
 */
#ifndef __TRANSLATION_MODEL_H__
#define __TRANSLATION_MODEL_H__

#include <vector>

class rule_tree;

class translation_model {
public:
    translation_model();
    virtual ~translation_model();

    translation_model(const translation_model&) = delete;
    translation_model& operator=(const translation_model&) = delete;

    unsigned int get_id() const;
    unsigned int get_feature_number() const;
    unsigned int get_index(unsigned int id) const;
    unsigned int get_feature_id(unsigned int index) const;

    void set_id(unsigned int id);
    void add_feature(unsigned int id);
private:
    unsigned int id;
    unsigned int feature_number;
    std::vector<unsigned int> feature_vector;
};

class rule_translation_model : public translation_model {
public:
    rule_translation_model();
    ~rule_translation_model();

    rule_translation_model(const rule_translation_model&) = delete;
    rule_translation_model& operator=(const rule_translation_model&) = delete;

    rule_tree* get_rule_tree() const;

    void add_rule_tree(rule_tree* tree);
private:
    unsigned int id;
    rule_tree* rule_table;
    unsigned int feature_number;
    std::vector<unsigned int> feature_vector;
};

#endif /* __TRANSLATION_MODEL_H__ */
