/*
 * cube.h
 * implementing cube pruning algorithm
 *
 * author: Playinf
 * email: playinf@stu.xmu.edu.cn
 *
 */
#ifndef __CUBE_H__
#define __CUBE_H__

#include <queue>
#include <unordered_set>
#include <functional.h>

class rule_list;
class hypothesis;

class cube {
public:
    typedef std::unordered_set<cube_item*, cube_item_hash, cube_item_equal_to>
        set_type;
    typedef std::priority_queue<cube_item*, std::vector<cube_item*>,
        cube_item_less> queue_type;

    cube(rule_list* list);
    ~cube();

    cube(const cube& c) = delete;
    cube& operator=(const cube& c) = delete;

    float get_best_score() const;

    cube_item* pop();
    bool empty() const;
    void set_margin(float margin);
private:
    void explore_neighbor(cube_item* item);
    void explore_neighbor(cube_item* item, unsigned int dim);

    float margin;
    unsigned int dimension;
    set_type enumerated_item_set;
    queue_type cube_item_queue;
};

class cube_queue {
public:
    typedef unsigned int size_type;
    typedef std::priority_queue<cube*, std::vector<cube*>, cube_less>
        queue_type;

    cube_queue();
    ~cube_queue();

    cube_queue(const cube_queue& q) = delete;
    cube_queue& operator=(const cube_queue& q) = delete;

    bool empty() const;
    size_type size() const;
    const hypothesis* pop();
    void insert(cube* rule_cube);
private:
    queue_type rule_cube_queue;
};

#endif /* __CUBE_H__ */
