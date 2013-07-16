/* task_queue.cpp */
#include <mutex>
#include <queue>
#include <thread>
#include <condition_variable>
#include <task.h>

task_queue::task_queue()
{
    limit = 0;
}

task_queue::~task_queue()
{
}

task* task_queue::get_task()
{
    task* t = nullptr;
    std::unique_lock<std::mutex> lock { mutual_exclusion };

    if (queue.empty()) {
        empty.wait(lock);
    }

    if (!queue.empty()) {
        t = queue.front();
        queue.pop();
    }

    lock.unlock();
    full.notify_all();

    return t;
}

unsigned int task_queue::size() const
{
    return queue.size();
}

void task_queue::set_limit(unsigned int limit)
{
    this->limit = limit;
}

void task_queue::add_task(task* translation_task)
{
    std::unique_lock<std::mutex> lock { mutual_exclusion };

    while (limit > 0 && queue.size() > limit) {
        full.wait(lock);
    }

    queue.push(translation_task);
    empty.notify_all();
    lock.unlock();
}
