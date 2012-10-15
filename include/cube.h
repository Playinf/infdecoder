/*
 * cube.h
 * implementing cube pruning algorithm
 *
 * author: Playinf
 * playinf@stu.xmu.edu.cn
 *
 */
#ifndef __CUBE_H__
#define __CUBE_H__

#include <queue>
#include <unordered_set>
#include <rule_set.h>
#include <cube_item.h>

struct cube_item_hash {

    size_t operator()(const cube_item* item) const
    {
        return item->hash();
    }

};

struct cube_item_equal_to {

    bool operator()(const cube_item* item1, const cube_item* item2) const
    {
        return *item1 == *item2;
    }

};

struct cube_item_less {

    bool operator()(const cube_item* item1, const cube_item* item2) const
    {
        return item1->get_score() < item2->get_score();
    }

};

class cube {
public:
    typedef std::unordered_set<cube_item*, cube_item_hash, cube_item_equal_to>
        set_type;
    typedef std::priority_queue<cube_item*, std::vector<cube_item*>, 
        cube_item_less> queue_type;

    cube(rule_list* list);
    ~cube();

    static void set_margin(double margin);

    cube_item* pop();
    double get_best_score() const;
    bool empty() const;
private:
    void explore_neighbor(cube_item* item);
    void explore_neighbor(cube_item* item, unsigned int dim);
    
    static double margin;
    unsigned int dimension;
    set_type enumerated_item_set;
    queue_type cube_item_queue;
};

struct cube_less {

    bool operator()(const cube* c1, const cube* c2) const
    {
        return c1->get_best_score() < c2->get_best_score();
    }

};

class cube_queue {
public:
    typedef unsigned int size_type;
    typedef std::priority_queue<cube*, std::vector<cube*>, cube_less> 
        queue_type;

    cube_queue();
    ~cube_queue();

    void insert(cube* rule_cube);
    const hypothesis* pop();
    bool empty() const;
    size_type size() const;
private:
    queue_type rule_cube_queue;
};

#endif /* __CUBE_H__ */
