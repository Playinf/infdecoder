/* task.cpp */
#include <mutex>
#include <queue>
#include <thread>
#include <sstream>
#include <condition_variable>
#include <task.h>
#include <model.h>
#include <buffer.h>
#include <config.h>
#include <decoder.h>
#include <trellis.h>
#include <utility.h>

#include <iostream>

task::task()
{
}

task::~task()
{
}

unsigned int task::get_id() const
{
    return id;
}

void task::run()
{
    decoder translator;

    std::cout << input << std::endl;

    //return;

    translator.process(input);
    hypothesis* best_hypo = translator.get_best_hypothesis();

    if (best_hypo == nullptr)
        return;

    std::string hypo_output;

    hypothesis_output_handler(best_hypo, hypo_output);
    translation_buffer->write(hypo_output, id);

    if (nbest_buffer == nullptr)
        return;

    std::vector<std::shared_ptr<const trellis_path>> path_list;
    translator.get_nbest_list(path_list);
    unsigned int size = path_list.size();
    std::stringstream out_stream;

    for (unsigned int i = 0; i < size; i++) {
        auto& path = path_list[i];
        std::string nbest_str;
        out_stream << std::to_string(id);
        out_stream << " ||| ";
        nbest_output_handler(path.get(), nbest_str);
        out_stream << nbest_str << std::endl;
    }

    out_stream << std::flush;

    nbest_buffer->write(out_stream.str(), id);
}

void task::set_id(unsigned int id)
{
    this->id = id;
}

void task::set_input(const std::string& sentence)
{
    input = sentence;
}
void task::set_nbest_buffer(output_buffer* buffer)
{
    nbest_buffer = buffer;
}
void task::set_translation_buffer(output_buffer *buffer)
{
    translation_buffer = buffer;
}

void task::set_handler(nbest_handler handler)
{
    nbest_output_handler = handler;
}

void task::set_handler(hypothesis_handler handler)
{
    hypothesis_output_handler = handler;
}
