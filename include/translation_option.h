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
#include <utility>

class rule;
class symbol;
class hypothesis;
class information;
class partial_rule;

class translation_option {
public:
    typedef unsigned int size_type;
    typedef std::vector<const rule*> vector_type;
    typedef std::vector<hypothesis*> hypothesis_list;

    translation_option(size_type size = 0);
    ~translation_option();

    translation_option(const translation_option&) = delete;
    translation_option& operator=(const translation_option&) = delete;

    size_type size() const;
    information* get_input() const;
    const partial_rule* get_rule() const;
    const symbol* get_start_symbol() const;
    size_type get_rule_nonterminal_number() const;
    std::vector<const rule*>* get_rule_vector() const;
    hypothesis_list* get_hypothesis_list(size_type index) const;
    std::pair<unsigned int, unsigned int> get_span() const;

    void set_rule(partial_rule* r);
    void set_input(information* input);
    void set_rule_vector(const vector_type* vec);
    void set_span(unsigned int start, unsigned int end);
    void add_hypothesis_list(unsigned int index, hypothesis_list* list);
private:
    information* input;
    const partial_rule* source_rule;
    std::vector<const rule*>* rule_vector;
    std::pair<unsigned int, unsigned int> span;
    std::vector<hypothesis_list*>* hypothesis_vector;
};

class translation_option_set {
public:
    typedef unsigned int size_type;

    translation_option_set();
    ~translation_option_set();

    translation_option_set(const translation_option_set&) = delete;
    translation_option_set& operator=(const translation_option_set&) = delete;

    translation_option* get_translation_option(size_type index) const;

    size_type size() const;

    void clear();
    void add_complete_rule(partial_rule* rule);
    void set_information(information* info);
    void set_span(unsigned int start, unsigned int end);
private:
    std::vector<translation_option*> applicable_rule_set;
};

#endif /* __TRANSLATION_OPTION_H__ */
