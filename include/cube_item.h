/*
 * cube_item.h
 * an item in a cube
 *
 * author: Playinf
 * playinf@stu.xmu.edu.cn
 *
 */
#ifndef __CUBE_ITEM_H__
#define __CUBE_ITEM_H__

#include <vector>
#include <rule.h>
#include <rule_set.h>
#include <hypothesis.h>

class rule_dimension {
public:
    rule_dimension(std::vector<const rule*>* rules, unsigned int pos)
    {
        sorted_rule_list = rules;
        position = pos;
    }

    ~rule_dimension()
    {
    }

    bool has_more_rules() const
    {
        unsigned int hypo_num = sorted_rule_list->size();

        if (position + 1 < hypo_num)
            return true;
        return false;
    }

    void next_rule()
    {
        position += 1;
    }

    const rule* get_rule() const
    {
        return sorted_rule_list->at(position);
    }

    bool operator==(const rule_dimension& dim) const
    {
        if (sorted_rule_list != dim.sorted_rule_list)
            return false;
        else if (position != dim.position)
            return false;

        return true;
    }

    bool operator!=(const rule_dimension& dim) const
    {
        return !operator==(dim);
    }

private:
    std::vector<const rule*>* sorted_rule_list;
    unsigned int position;
};

class hypothesis_dimension {
public:
    hypothesis_dimension(std::vector<hypothesis*>* hypo, unsigned int num)
    {
        sorted_hypothesis_list = hypo;
        position = num;
    }

    ~hypothesis_dimension()
    {
    }

    bool has_more_hypothesis() const
    {
        unsigned int rule_num = sorted_hypothesis_list->size();

        if (position + 1 < rule_num)
            return true;
        return false;
    }

    void next_hypothesis()
    {
        position += 1;
    }

    const hypothesis* get_hypothesis() const
    {
        return sorted_hypothesis_list->at(position);
    }

    bool operator==(const hypothesis_dimension& dim) const
    {
        if (sorted_hypothesis_list != dim.sorted_hypothesis_list)
            return false;
        else if (position != dim.position)
            return false;

        return true;
    }

    bool operator!=(const hypothesis_dimension& dim)
    {
        return !operator==(dim);
    }

private:
    std::vector<hypothesis*>* sorted_hypothesis_list;
    unsigned int position;
};

class cube_item {
public:
    cube_item(rule_list* list);
    cube_item(const cube_item& item, unsigned int dim);
    ~cube_item();

    double get_score() const;
    void generate_hypothesis();
    hypothesis* get_hypothesis();
    unsigned int get_hypothesis_dimension() const;
    bool has_more_rules() const;
    bool has_more_hypotheses(unsigned int dim) const;
    bool operator==(const cube_item& item) const;
    unsigned int hash() const;
private:
    rule_dimension rule_position;
    std::vector<hypothesis_dimension>* hypothesis_position;
    double score;
    hypothesis* generated_hypothesis;
};

#endif /* __CUBE_ITEM_H__ */
