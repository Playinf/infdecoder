/* thread_manager.cpp */
#include <thread>
#include <vector>
#include <task.h>

#include <iostream>

task_manager::task_manager(unsigned int thread_number)
{
    limit = 0;
    stop_flag = false;
    this->thread_number = thread_number;

    for (unsigned int i = 0; i < thread_number; i++) {
        std::thread* t;
        t = new std::thread(std::bind(&task_manager::execute, this));
        thread_vector.push_back(t);
    }
}

task_manager::~task_manager()
{
    for (unsigned int i = 0; i < thread_number; i++) {
        std::thread* t = thread_vector[i];
        delete t;
    }
}

bool task_manager::get_stop_flag() const
{
    return stop_flag;
}

unsigned int task_manager::get_task_number() const
{
    return task_queue.size();
}

void task_manager::stop()
{
    std::unique_lock<std::mutex> lock { mutual_exclusion };

    while (!task_queue.empty() && !stop_flag)
        thread_available.wait(lock);

    stop_flag = true;
    lock.unlock();
    thread_needed.notify_all();

    for (unsigned int i = 0; i < thread_number; i++) {
        std::thread* t = thread_vector[i];
        if (t->joinable())
            t->join();
    }
}

void task_manager::add_task(task* translation_task)
{
    std::unique_lock<std::mutex> lock { mutual_exclusion };

    while (limit > 0 && task_queue.size() >= limit)
        thread_available.wait(lock);

    task_queue.push(translation_task);
    thread_needed.notify_all();
}

void task_manager::execute()
{
    do {
        task * t = nullptr;
        std::unique_lock<std::mutex> lock { mutual_exclusion };

        if (task_queue.empty() && !stop_flag) {
            thread_needed.wait(lock);
        }

        if (!task_queue.empty() && !stop_flag) {
            t = task_queue.front();
            task_queue.pop();
        }

        lock.unlock();

        if (t != nullptr) {
            t->run();
            delete t;
        }

        thread_available.notify_all();
    } while (!stop_flag);
}
