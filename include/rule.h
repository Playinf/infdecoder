/*
 * rule.h
 *
 *
 * author: Playinf
 * email: playinf@stu.xmu.edu.cn
 *
 */
#ifndef __RULE_H__
#define __RULE_H__

#include <map>
#include <vector>
#include <utility>
#include <lm.h>
#include <symbol.h>
#include <feature.h>

class rule {
public:
    typedef unsigned int size_type;

    rule(size_type size)
    {
        start_symbol = nullptr;
        score = 0.0f;
        reorder = false;
        target_terminal_num = 0;
        target_rule_body.reserve(size);
    }

    ~rule()
    {
        // do nothing
    }

    void add_symbol(const symbol* sym)
    {
        target_rule_body.push_back(sym);

        if (sym->is_terminal())
            target_terminal_num++;
    }

    void set_order(bool reorder)
    {
        this->reorder = reorder;
    }

    void set_start_symbol(const symbol* sym)
    {
        start_symbol = sym;
    }

    const symbol* get_start_symbol() const
    {
        return start_symbol;
    }

    size_type get_terminal_num() const
    {
        return target_terminal_num;
    }

    bool reorder_nonterminal() const
    {
        return reorder;
    }

    void evaluate_score();

    float get_score() const
    {
        return score;
    }

    const std::vector<const symbol*>& get_target_rule_body() const
    {
        return target_rule_body;
    }

    const feature* get_feature(unsigned int id) const
    {
        unsigned int size = feature_set.size();

        for (unsigned int i = 0; i < size; i++) {
            const feature& f = feature_set[i];

            if (f.get_id() == id)
                return &f;
        }

        return nullptr;
    }

    void add_feature(feature* fea)
    {
        feature_set.push_back(*fea);
    }

    static void push_weight(double weight)
    {
        lambda.push_back(weight);
    }

private:
    float score;
    bool reorder;
    const symbol* start_symbol;
    std::vector<feature> feature_set;
    std::vector<const symbol*> target_rule_body;
    unsigned int target_terminal_num;
    static std::vector<float> lambda;
};

struct rule_less {
    bool operator()(const rule* r1, const rule* r2) const
    {
        return r1->get_score() > r2->get_score();
    }
};

#endif /* __RULE_H__ */
