/* thread_manager.cpp */
#include <thread>
#include <vector>
#include <task.h>

static void execute(task_queue* q, task_manager* manager)
{
    while (!(q->size() == 0 && manager->get_stop_flag() == true)) {
        task* t = q->get_task();

        if (t == nullptr)
            continue;

        t->run();
        delete t;
    }
}

task_manager::task_manager(unsigned int thread_number)
{
    stop_flag = false;
    this->thread_number = thread_number;

    for (unsigned int i = 0; i < thread_number; i++) {
        std::thread* t = new std::thread(execute, &queue, this);
        thread_vector.push_back(t);
    }
}

task_manager::~task_manager()
{
    for (unsigned int i = 0; i < thread_number; i++) {
        std::thread* t = thread_vector[i];
        t->join();
        delete t;
    }
}

bool task_manager::get_stop_flag() const
{
    return stop_flag;
}

unsigned int task_manager::get_task_number() const
{
    return queue.size();
}

void task_manager::stop()
{
    stop_flag = true;
}

void task_manager::add_task(task* translation_task)
{
    queue.add_task(translation_task);
}
