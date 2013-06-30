/* functional.cpp */
#include <rule.h>
#include <symbol.h>
#include <rule_tree.h>
#include <functional.h>

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

bool rule_less::operator()(const rule* r1, const rule* r2) const
{
    return r1->get_score() > r2->get_score();
}

size_t rt_node_hash::operator()(const rt_node& node) const
{
    symbol_address_hash hasher;

    return hasher(node.get_symbol(0));
}
