/* test_task_queue.cpp */
#include <iostream>
#include <task.h>
#include <buffer.h>

void execute(task_manager* q, output_buffer* out_buf)
{
    task* t = q->get_task();

    while (t != nullptr) {
        std::string str = "excute task ";
        str += std::to_string(t->get_id());
        str += "\n";
        out_buf->write(str, t->get_id());
        delete t;
        t = q->get_task();
    }
}

int main()
{
    task_manager q;
    unsigned int id = 0;
    output_buffer buffer(&std::cout);
    std::thread t1 { execute, &q, &buffer };
    std::thread t2 { execute, &q, &buffer };
    std::thread t3 { execute, &q, &buffer };

    q.set_limit(1);

    while (true)
    {
        task* t = new task();
        t->set_id(id++);
        q.add_task(t);
    }

    t1.join();
    t2.join();
    t3.join();
}
