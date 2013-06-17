/*
 * feature_function.h
 *
 *
 * author: Playinf
 * email: playinf@stu.xmu.edu.cn
 *
 */
#ifndef __FEATURE_FUNCTION_H__
#define __FEATURE_FUNCTION_H__

class hypothesis;

double translation_feature_function(const hypothesis* h, unsigned int id);
double lm_feature_function(const hypothesis* h, unsigned int id);
double word_penalty_feature_function(const hypothesis* h, unsigned int id);
double extracted_rule_feature_function(const hypothesis* h, unsigned int id);
double glue_rule_feature_function(const hypothesis* h, unsigned int id);

#endif /* __FEATURE_FUNCTION_H__ */
