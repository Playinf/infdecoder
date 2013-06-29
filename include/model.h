/*
 * model.h
 *
 *
 * author: Playinf
 * email: playinf@stu.xmu.edu.cn
 *
 */
#ifndef __MODEL_H__
#define __MODEL_H__

#include <map>
#include <vector>
#include <feature.h>

class model {
public:
    model()
    {
        feature_set.reserve(feature_num);

        for (unsigned int i = 0; i < feature_num; i++)
            feature_set.push_back(feature(i, function_vector[i]));
    }

    ~model()
    {
    }

    static void add_feature(const std::string& des, feature::feature_function func);

    static feature::feature_function get_feature_function(unsigned int id)
    {
        return function_vector[id];
    }

    static unsigned int get_feature_id(const std::string& des)
    {
        return feature_id_map[des];
    }

    static void set_weight(std::vector<double>& weight)
    {
        lambda.reserve(feature_num);

        for (unsigned int i = 0; i < feature_num; i++)
            lambda.push_back(weight[i]);
    }

    static double get_weight(unsigned int id)
    {
        return lambda[id];
    }

    feature* get_feature(unsigned int id) const
    {
        const feature* fea = &feature_set[id];
        return const_cast<feature*>(fea);
    }

    unsigned int get_feature_num() const
    {
        return feature_num;
    }

    double get_score() const
    {
        double score = 0.0;

        for (unsigned int i = 0; i < feature_num; i++)
            score += lambda[i] * feature_set[i].get_score();

        return score;
    }

    static double get_lm_lambda()
    {
        return lm_lambda;
    }

    static void set_lm_lambda(double weight)
    {
        lm_lambda = weight;
    }

private:
    std::vector<feature> feature_set;
    static std::vector<double> lambda;
    static double lm_lambda;
    static unsigned int feature_id;
    static unsigned int feature_num;
    static std::map<std::string, unsigned int> feature_id_map;
    static std::vector<feature::feature_function> function_vector;
};

#endif /* __MODEL_H__ */
