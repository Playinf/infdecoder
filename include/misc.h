/*
 * misc.h
 *
 * author: Playinf
 * email: playinf@stu.xmu.edu.cn
 */
#ifndef __MISC_H__
#define __MISC_H__

#include <string>
#include <vector>
#include <utility>

typedef std::pair<unsigned int, unsigned int> align_type;

class hypothesis;
class trellis_node;
class trellis_path;

void output_alignment(const hypothesis* h, unsigned int i,
    std::vector<align_type>& a);
void output_alignment(const trellis_node* n, unsigned int i,
    std::vector<align_type>& a);
void output_full_alignment(const hypothesis* h, std::vector<align_type>& a);
void output_full_alignment(const trellis_path* p, std::vector<align_type>& a);

#endif /* __MISC_H__ */
