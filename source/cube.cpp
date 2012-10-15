/* cube.h */
#include <cube.h>

double cube::margin;

cube::cube(rule_list* list)
{
    cube_item* item = new cube_item(list);
    dimension = list->get_rule_nonterminal_num() + 1;

    item->generate_hypothesis();
    enumerated_item_set.insert(item);
    cube_item_queue.push(item);
}

cube::~cube()
{
    auto iter = enumerated_item_set.begin();
    auto end = enumerated_item_set.end();

    while (iter != end) {
        cube_item* item = *iter;
        delete item;
        iter++;
    }
}

void cube::set_margin(double val)
{
    margin = val;
}

bool cube::empty() const
{
    return cube_item_queue.empty();
}

cube_item* cube::pop()
{
    cube_item* item = cube_item_queue.top();
    cube_item_queue.pop();

    explore_neighbor(item);

    return item;
}

double cube::get_best_score() const
{
    cube_item* item = cube_item_queue.top();

    return item->get_score();
}

void cube::explore_neighbor(cube_item* item)
{
    if (item->has_more_rules())
        explore_neighbor(item, -1);

    for (unsigned int i = 1; i < dimension; i++)
        if (item->has_more_hypotheses(i - 1))
            explore_neighbor(item, i - 1);
}

void cube::explore_neighbor(cube_item* item, unsigned int dim)
{
    cube_item* new_item = new cube_item(*item, dim);

    new_item->generate_hypothesis();
    auto result = enumerated_item_set.insert(new_item);

    if (result.second) {
        cube_item_queue.push(new_item);
    } else {
        delete new_item;
    }
}

cube_queue::cube_queue()
{
}

cube_queue::~cube_queue()
{
    while (!rule_cube_queue.empty()) {
        cube* rule_cube = rule_cube_queue.top();
        rule_cube_queue.pop();
        delete rule_cube;
    }
}

void cube_queue::insert(cube* rule_cube)
{
    rule_cube_queue.push(rule_cube);
}

const hypothesis* cube_queue::pop()
{
    hypothesis* hypo;
    cube* rule_cube = rule_cube_queue.top();
    rule_cube_queue.pop();

    hypo = rule_cube->pop()->get_hypothesis();

    if (!rule_cube->empty())
        rule_cube_queue.push(rule_cube);
    else
        delete rule_cube;

    return hypo;
}

bool cube_queue::empty() const
{
    return rule_cube_queue.empty();
}

cube_queue::size_type cube_queue::size() const
{
    return rule_cube_queue.size();
}
