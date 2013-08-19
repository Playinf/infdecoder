/* alignment.cpp */
#include <vector>
#include <utility>
#include <functional>
#include <utility.h>
#include <alignment.h>
#include <functional.h>

alignment::alignment(unsigned int size, unsigned int num)
{
    alignment_vector.reserve(size);
    nonterminal_map.resize(num);
}

alignment::~alignment()
{
    /* do nothing */
}

const std::vector<alignment::align_type>& alignment::get_alignment() const
{
    return alignment_vector;
}

unsigned int alignment::get_nonterminal_map(unsigned int position) const
{
    return nonterminal_map[position];
}

unsigned int alignment::get_source_position(unsigned int position) const
{
    unsigned int size = alignment_vector.size();
    const unsigned int not_found = static_cast<unsigned int>(-1);

    for (unsigned int i = 0; i < size; i++) {
        auto& p = alignment_vector[i];

        if (p.second == position)
            return p.first;
    }

    return not_found;
}

unsigned int alignment::get_target_position(unsigned int position) const
{
    unsigned int size = alignment_vector.size();
    const unsigned int not_found = static_cast<unsigned int>(-1);

    for (unsigned int i = 0; i < size; i++) {
        auto& p = alignment_vector[i];

        if (p.first == position)
            return p.second;
    }

    return not_found;
}

std::size_t alignment::hash() const
{
    unsigned int s1 = alignment_vector.size();
    unsigned int s2 = nonterminal_map.size();
    std::size_t hash_code = 0;

    for (unsigned int i = 0; i < s1; i++) {
        auto& p = alignment_vector[i];
        hash_code = hash_combine(hash_code, p.first);
        hash_code = hash_combine(hash_code, p.second);
    }

    for (unsigned int i = 0; i < s2; i++) {
        unsigned int val = nonterminal_map[i];
        hash_code = hash_combine(hash_code, val);
    }

    return hash_code;
}

int alignment::compare(const alignment& a) const
{
    auto& nonterm_map = a.nonterminal_map;
    auto& align_vec = a.alignment_vector;
    unsigned int size1 = nonterminal_map.size();
    unsigned int size2 = nonterm_map.size();

    unsigned int size;

    if (size1 != size2)
        return size1 - size2;

    size = size1;

    for (unsigned int i = 0; i < size; i++) {
        unsigned int index1 = nonterminal_map[i];
        unsigned int index2 = nonterm_map[i];

        if (index1 != index2)
            return index1 - index2;
    }

    size1 = alignment_vector.size();
    size2 = align_vec.size();

    if (size1  != size2)
        return size1 - size2;

    size = size1;

    for (unsigned int i = 0; i < size; i++) {
        auto& p1 = alignment_vector[i];
        auto& p2 = align_vec[i];

        if (p1.first != p2.first)
            return p1.first - p2.first;

        if (p1.second != p2.second)
            return p1.second - p2.second;
    }

    return 0;
}

void alignment::add_alignment(unsigned int source, unsigned int target)
{
    alignment_vector.push_back(std::make_pair(source, target));
}

void alignment::add_nonterminal_map(unsigned int source, unsigned int target)
{
    nonterminal_map[source] = target;
}
