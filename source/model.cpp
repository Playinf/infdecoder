/* model.cpp */
#include <utility>
#include <model.h>

/* TODO:
 * be careful about static variables they may result in bugs in multi-threaded
 * environment
 */
unsigned int model::feature_id = 0;
unsigned int model::feature_num = 0;
double model::lm_lambda = 0.0;
std::vector<double> model::lambda;
std::map<std::string, unsigned int> model::feature_id_map;
std::vector<feature::feature_function> model::function_vector;

void model::add_feature(const std::string& des, feature::feature_function func)
{
    auto result = feature_id_map.insert(std::make_pair(des, feature_id));

    if (!result.second)
        return;

    feature_num++;
    feature_id++;
    function_vector.push_back(func);
}
