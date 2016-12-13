/*
 * buffer.h
 *
 *
 * author: Playinf
 * email: playinf@stu.xmu.edu.cn
 *
 */
#ifndef __BUFFER_H__
#define __BUFFER_H__

#include <map>
#include <mutex>
#include <string>
#include <ostream>

class output_buffer {
public:
    output_buffer(std::ostream* stream);
    ~output_buffer();

    output_buffer(const output_buffer&) = delete;
    output_buffer& operator=(const output_buffer&) = delete;

    void write(const std::string& str, unsigned int id);
private:
    unsigned int expected_id;
    std::ostream* output_stream;
    std::mutex mutual_exclusion;
    std::map<unsigned int, std::string> output_map;
};

#endif /* __BUFFER_H__ */
