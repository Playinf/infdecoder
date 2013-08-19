/*
 * alignment.h
 *
 *
 * author: Playinf
 * email: playinf@stu.xmu.edu.cn
 *
 */
#ifndef __ALIGNMENT_H__
#define __ALIGNMENT_H__

#include <vector>
#include <utility>
#include <unordered_set>
#include <functional.h>
#include <shared_mutex.h>

class alignment {
public:
    typedef std::pair<unsigned int, unsigned int> align_type;
    alignment(unsigned int size, unsigned int num);
    ~alignment();

    alignment& operator=(const alignment& a) = delete;

    const std::vector<align_type>& get_alignment() const;
    unsigned int get_nonterminal_map(unsigned int position) const;
    unsigned int get_source_position(unsigned int position) const;
    unsigned int get_target_position(unsigned int position) const;

    std::size_t hash() const;
    int compare(const alignment& a) const;
    void add_alignment(unsigned int source, unsigned int target);
    void add_nonterminal_map(unsigned int source, unsigned int target);
private:
    std::vector<unsigned int> nonterminal_map;
    std::vector<std::pair<unsigned int, unsigned int>> alignment_vector;
};

class alignment_table {
public:
    ~alignment_table();

    static alignment_table* get_instance();

    const alignment* search_alignment(const alignment& align);
private:
    alignment_table();

    shared_mutex mutex;
    std::unordered_set<alignment, alignment_hash, alignment_equal> align_set;
    static alignment_table instance;
};

#endif /* __ALIGNMENT_H__ */
