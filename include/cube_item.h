/*
 * cube_item.h
 * an item in a cube
 *
 * author: Playinf
 * email: playinf@stu.xmu.edu.cn
 *
 */
#ifndef __CUBE_ITEM_H__
#define __CUBE_ITEM_H__

#include <vector>

class rule;
class hypothesis;
class translation_option;

class rule_dimension {
public:
    rule_dimension(std::vector<const rule*>* rules, unsigned int pos);
    ~rule_dimension();

    rule_dimension& operator=(const rule_dimension& r) = delete;

    const rule* get_rule() const;
    unsigned int get_position() const;

    bool has_more_rules() const;
    void next_rule();
private:
    std::vector<const rule*>* sorted_rule_list;
    unsigned int position;
};

class hypothesis_dimension {
public:
    hypothesis_dimension(std::vector<hypothesis*>* hypo, unsigned int num);
    ~hypothesis_dimension();

    hypothesis_dimension& operator=(const hypothesis_dimension& h) = delete;

    unsigned int get_position() const;
    const hypothesis* get_hypothesis() const;

    bool has_more_hypothesis() const;
    void next_hypothesis();
private:
    std::vector<hypothesis*>* sorted_hypothesis_list;
    unsigned int position;
};

class cube_item {
public:
    cube_item(translation_option* option);
    cube_item(const cube_item& item, unsigned int dim);
    ~cube_item();

    float get_score() const;
    hypothesis* get_hypothesis();
    unsigned int get_rule_position() const;
    unsigned int get_hypothesis_dimension() const;
    unsigned int get_hypothesis_position(unsigned int dim) const;

    void generate_hypothesis();
    bool has_more_rules() const;
    bool has_more_hypotheses(unsigned int dim) const;
    bool operator==(const cube_item& item) const;
    unsigned int hash() const;
private:
    float score;
    rule_dimension rule_position;
    std::vector<hypothesis_dimension>* hypothesis_position;
    hypothesis* generated_hypothesis;
};

#endif /* __CUBE_ITEM_H__ */
