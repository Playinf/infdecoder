/* cube_queue.cpp */
#include <cube.h>
#include <cube_item.h>

cube_queue::cube_queue()
{
    // do nothing
}

cube_queue::~cube_queue()
{
    while (!rule_cube_queue.empty()) {
        cube* rule_cube = rule_cube_queue.top();
        rule_cube_queue.pop();
        delete rule_cube;
    }
}

bool cube_queue::empty() const
{
    return rule_cube_queue.empty();
}

cube_queue::size_type cube_queue::size() const
{
    return rule_cube_queue.size();
}

const hypothesis* cube_queue::pop()
{
    hypothesis* hypo;
    cube* rule_cube = rule_cube_queue.top();
    cube_item* item = rule_cube->pop();

    rule_cube_queue.pop();
    hypo = item->get_hypothesis();

    if (!rule_cube->empty())
        rule_cube_queue.push(rule_cube);
    else
        delete rule_cube;

    return hypo;
}

void cube_queue::insert(cube* rule_cube)
{
    rule_cube_queue.push(rule_cube);
}
