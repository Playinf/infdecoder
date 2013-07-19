/* buffer.cpp */
#include <mutex>
#include <string>
#include <ostream>
#include <buffer.h>

output_buffer::output_buffer(std::ostream* stream)
{
    expected_id = 0;
    output_stream = stream;
}

output_buffer::~output_buffer()
{
    /* do nothing */
}

void output_buffer::write(const std::string& str, unsigned int id)
{
    std::unique_lock<std::mutex> lock { mutual_exclusion };

    if (id == expected_id) {
        *output_stream << str << std::flush;
        expected_id++;
    } else {
        output_map[id] = str;
        return;
    }

    while (true) {
        auto iter = output_map.find(expected_id);

        if (iter == output_map.end())
            break;

        *output_stream << iter->second << std::flush;
        expected_id++;
    }
}
