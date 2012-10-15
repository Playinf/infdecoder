/*
 * buffer.h
 *
 *
 * author: Playinf
 * playinf@stu.xmu.edu.cn
 *
 */
#ifndef __BUFFER_H__
#define __BUFFER_H__

#include <queue>
#include <vector>
#include <symbol.h>
#include <semaphore.h>

class input_buffer {
public:
    typedef unsigned int size_type;
    typedef std::vector<const symbol*> input_type;
private:
    std::queue<input_type*> buffer;
    size_type buffer_size;
    
};

class output_buffer {
public:
    typedef unsigned int size_type;
private:
    size_type buffer_type;
};

#endif /* __BUFFER_H__ */
