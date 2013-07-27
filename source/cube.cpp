/* cube.h */
#include <cube.h>
#include <cube_item.h>
#include <translation_option.h>

cube::cube(translation_option* option)
{
    cube_item* item = new cube_item(option);
    dimension = option->get_rule_nonterminal_number() + 1;

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
        ++iter;
    }
}

float cube::get_best_score() const
{
    cube_item* item = cube_item_queue.top();

    return item->get_score();
}

cube_item* cube::pop()
{
    cube_item* item = cube_item_queue.top();
    cube_item_queue.pop();

    explore_neighbor(item);

    return item;
}

bool cube::empty() const
{
    return cube_item_queue.empty();
}

void cube::explore_neighbor(cube_item* item)
{
    const unsigned int rule_dim = 0;

    if (item->has_more_rules())
        explore_neighbor(item, rule_dim);

    for (unsigned int i = 0; i < dimension - 1; i++)
        if (item->has_more_hypotheses(i))
            explore_neighbor(item, i + 1);
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
