/* functional.cpp */
#include <cube.h>
#include <rule.h>
#include <symbol.h>
#include <trellis.h>
#include <cube_item.h>
#include <rule_tree.h>
#include <functional.h>
#include <hypothesis.h>

std::size_t symbol_hash::operator()(const symbol& sym) const
{
    std::hash<std::string> hasher;

    return hasher(*sym.get_name());
}

std::size_t symbol_address_hash::operator()(const symbol* sym) const
{
    std::hash<const symbol*> hasher;

    return hasher(sym);
}

std::size_t symbol_equal::operator()(const symbol& s1, const symbol& s2) const
{
    const std::string& str1 = *s1.get_name();
    const std::string& str2 = *s2.get_name();

    if (str1 != str2)
        return false;

    return s1.get_type() == s2.get_type();
}

/* sort in descending order */
bool rule_less::operator()(const rule* r1, const rule* r2) const
{
    return r1->get_heuristic_score() > r2->get_heuristic_score();
}

std::size_t rt_node_hash::operator()(const rt_node& node) const
{
    symbol_address_hash hasher;

    return hasher(node.get_symbol(0));
}

std::size_t cube_item_hash::operator()(const cube_item* item) const
{
    return item->hash();
}

bool cube_item_equal_to::operator()(const cube_item* item1,
    const cube_item* item2) const
{
    return *item1 == *item2;
}

bool cube_item_less::operator()(const cube_item* c1, const cube_item* c2) const
{
    return c1->get_score() < c2->get_score();
}

bool cube_less::operator()(const cube* c1, const cube* c2) const
{
    return c1->get_best_score() < c2->get_best_score();
}

/* sort in descending order */
bool hypothesis_less::operator()(const hypothesis* h1,
    const hypothesis* h2) const
{
    float s1 = h1->get_total_score();
    float s2 = h2->get_total_score();

    return s1 > s2;
}

bool hypothesis_compare::operator()(const hypothesis* h1,
    const hypothesis* h2) const
{
    int ret = h1->compare(h2);

    if (ret != 0)
        return ret < 0;

    return false;
}

bool trellis_detour_less::operator()(const trellis_detour* d1,
    const trellis_detour* d2) const
{
    return d1->get_total_score() < d2->get_total_score();
}
