/* alignment_table.cpp */
#include <unordered_set>
#include <alignment.h>
#include <shared_mutex.h>

// global object
alignment_table alignment_table::instance;

alignment_table::alignment_table()
{
    // do nothing
}

alignment_table::~alignment_table()
{
    // do nothing
}

alignment_table* alignment_table::get_instance()
{
    return &instance;
}

const alignment* alignment_table::search_alignment(const alignment& align)
{
    mutex.lock_shared();
    auto iter = align_set.find(align);
    mutex.unlock_shared();

    if (iter != align_set.end())
        return &(*iter);


    mutex.lock();
    auto result = align_set.insert(align);
    mutex.unlock();

    return &(*result.first);
}
