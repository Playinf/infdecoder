/*
 * task.h
 *
 *
 * author: Playinf
 * email: playinf@stu.xmu.edu.cn
 *
 */
#ifndef __TASK_H__
#define __TASK_H__

#include <mutex>
#include <queue>
#include <thread>
#include <condition_variable>
#include <handler.h>

class output_buffer;

class task {
public:
    task();
    ~task();

    task(const task& t) = delete;
    task& operator=(const task& t) = delete;

    unsigned int get_id() const;

    void run();
    void set_id(unsigned int id);
    void set_input(const std::string& sentence);
    void set_nbest_buffer(output_buffer* buffer);
    void set_verbose_buffer(output_buffer* buffer);
    void set_translation_buffer(output_buffer *buffer);
    void set_handler(nbest_handler handler);
    void set_handler(hypothesis_handler handler);
    void set_verbose_handler(hypothesis_handler handler);
private:
    unsigned int id;
    std::string input;
    output_buffer* nbest_buffer;
    output_buffer* verbose_buffer;
    output_buffer* translation_buffer;
    hypothesis_handler hypothesis_output_handler;
    hypothesis_handler hypothesis_track_handler;
    nbest_handler nbest_output_handler;
};

class task_manager {
public:
    explicit task_manager(unsigned int thread_number);
    ~task_manager();

    task_manager(const task_manager& manager) = delete;
    task_manager& operator=(const task_manager& manager) = delete;

    bool get_stop_flag() const;
    unsigned int get_task_number() const;

    void stop();
    void set_limit(unsigned int limit);
    void add_task(task* translation_task);
private:
    void execute();

    bool stop_flag;
    unsigned int limit;
    std::queue<task*> task_queue;
    std::mutex mutual_exclusion;
    std::condition_variable thread_needed;
    std::condition_variable thread_available;
    unsigned int thread_number;
    std::vector<std::thread*> thread_vector;
};


#endif /* __TASK_H__ */
