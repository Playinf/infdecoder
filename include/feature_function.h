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

typedef float (*feature_function)(const hypothesis* h, unsigned int id);

float language_model_feature_function(const hypothesis* h, unsigned int id);
float translation_model_feature_function(const hypothesis* h, unsigned int id);
float word_penalty_feature_function(const hypothesis* h, unsigned int id);

#endif /* __FEATURE_FUNCTION_H__ */
