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
    index_map[0].resize(num);
    index_map[1].resize(num);
    nonterminal_map[0].resize(num);
    nonterminal_map[1].resize(num);
    nonterminal_number = num;
}

alignment::~alignment()
{
    // do nothing
}

const std::vector<alignment::align_type>& alignment::get_alignment() const
{
    return alignment_vector;
}

unsigned int alignment::get_nonterminal_map(unsigned int position) const
{
    return nonterminal_map[0][position];
}

unsigned int alignment::get_nonterminal_map(unsigned int pos, unsigned int dir)
    const
{
    return nonterminal_map[dir][pos];
}

unsigned int alignment::get_source_nonterminal_index(unsigned int pos) const
{
    for (unsigned int i = 0; i < nonterminal_number; i++) {
        unsigned int position = index_map[0][i];

        if (position == pos)
            return i;
    }

    return static_cast<unsigned int>(-1);
}

unsigned int alignment::get_target_nonterminal_index(unsigned int pos) const
{
    for (unsigned int i = 0; i < nonterminal_number; i++) {
        unsigned int position = index_map[1][i];

        if (position == pos)
            return i;
    }

    return static_cast<unsigned int>(-1);
}

unsigned int alignment::get_source_nonterminal_position(unsigned int idx) const
{
    if (idx >= nonterminal_number)
        return static_cast<unsigned int>(-1);

    return index_map[0][idx];
}

unsigned int alignment::get_target_nonterminal_position(unsigned int idx) const
{
    if (idx >= nonterminal_number)
        return static_cast<unsigned int>(-1);

    return index_map[1][idx];
}

std::size_t alignment::hash() const
{
    std::size_t hash_code = 0;

    for (auto& val : alignment_vector) {
        hash_code = hash_combine(hash_code, val.first);
        hash_code = hash_combine(hash_code, val.second);
    }

    // only need source side nonterminal order
    for (auto& val : nonterminal_map[0]) {
        hash_code = hash_combine(hash_code, val);
    }

    for (auto& val : index_map[0]) {
        hash_code = hash_combine(hash_code, val);
    }

    for (auto& val : index_map[1]) {
        hash_code = hash_combine(hash_code, val);
    }

    return hash_code;
}

int alignment::compare(const alignment& a) const
{
    auto& align_vec = a.alignment_vector;
    unsigned int size1 = nonterminal_number;
    unsigned int size2 = a.nonterminal_number;

    if (size1 != size2)
        return size1 - size2;

    size1 = alignment_vector.size();
    size2 = align_vec.size();

    if (size1  != size2)
        return size1 - size2;

    unsigned size = size1;

    // compare alignment
    for (unsigned int i = 0; i < size; i++) {
        auto& p1 = alignment_vector[i];
        auto& p2 = align_vec[i];

        if (p1.first != p2.first)
            return p1.first - p2.first;

        if (p1.second != p2.second)
            return p1.second - p2.second;
    }

    // compare nonterminal order, only need source side
    for (unsigned int i = 0; i < nonterminal_number; i++) {
        unsigned int pos1 = nonterminal_map[0][i];
        unsigned int pos2 = a.nonterminal_map[0][i];

        if (pos1 != pos2)
            return pos1 - pos2;
    }

    // compare index map
    for (unsigned int i = 0; i < nonterminal_number; i++) {
        unsigned int pos1 = index_map[0][i];
        unsigned int pos2 = a.index_map[0][i];

        if (pos1 != pos2)
            return pos1 - pos2;
    }

    // compare index map
    for (unsigned int i = 0; i < nonterminal_number; i++) {
        unsigned int pos1 = index_map[1][i];
        unsigned int pos2 = a.index_map[1][i];

        if (pos1 != pos2)
            return pos1 - pos2;
    }

    return 0;
}

void alignment::add_alignment(unsigned int source, unsigned int target)
{
    alignment_vector.push_back(std::make_pair(source, target));
}

void alignment::add_nonterminal_map(unsigned int source, unsigned int target)
{
    nonterminal_map[0][source] = target;
    nonterminal_map[1][target] = source;
}

 void alignment::add_index_map(unsigned int p, unsigned int i, unsigned int d)
 {
     index_map[d][i] = p;
 }
