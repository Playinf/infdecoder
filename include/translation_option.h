/*
 * translation_option.h
 *
 * author: Playinf
 * email: playinf@stu.xmu.edu.cn
 *
 */
#ifndef __TRANSLATION_OPTION_H__
#define __TRANSLATION_OPTION_H__

#include <vector>

class rule;
class symbol;
class hypothesis;
class partial_rule;

class translation_option {
public:
    typedef unsigned int size_type;
    typedef std::vector<const rule*> vector_type;
    typedef std::vector<hypothesis*> hypothesis_list;

    translation_option(size_type size = 0);
    ~translation_option();

    translation_option(const translation_option& r) = delete;
    translation_option& operator=(const translation_option& r) = delete;

    const partial_rule* get_rule() const;
    const symbol* get_start_symbol() const;
    size_type get_rule_nonterminal_number() const;
    std::vector<const rule*>* get_rule_vector() const;
    hypothesis_list* get_hypothesis_vector(size_type index);

    size_type size() const;
    void set_rule(partial_rule* r);
    void set_rule_vector(const vector_type* vec);
    void add_hypothesis_list(unsigned int index, hypothesis_list* list);
private:
    const partial_rule* source_rule;
    std::vector<const rule*>* rule_vector;
    std::vector<hypothesis_list*>* hypothesis_vector;
};

class translation_option_set {
public:
    typedef unsigned int size_type;

    translation_option_set();
    ~translation_option_set();

    translation_option_set(const translation_option_set& r) = delete;
    translation_option_set& operator=(const translation_option_set& s) = delete;

    translation_option* get_translation_option(size_type index) const;

    void clear();
    size_type size() const;
    void add_complete_rule(partial_rule* rule);
private:
    std::vector<translation_option*> applicable_rule_set;
};

#endif /* __TRANSLATION_OPTION_H__ */
