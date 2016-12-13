/*
 * rescore_function.h
 *
 * author: Playinf
 * email: playinf@stu.xmu.edu.cn
 *
 */
#ifndef __RESCORE_FUNCTION_H__
#define __RESCORE_FUNCTION_H__

class hypothesis;

float joint_model_rescore_function(const hypothesis* h, unsigned int id);
float lexical_model_rescore_function(const hypothesis* h, unsigned int id);
float language_model_rescore_function(const hypothesis* h, unsigned int id);
float translation_model_rescore_function(const hypothesis* h, unsigned int id);

#endif /* __RESCORE_FUNCTION_H__ */
