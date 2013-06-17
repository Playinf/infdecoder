/*
 * derivation.h
 *
 *
 * author: Playinf
 * email: playinf@stu.xmu.edu.cn
 *
*/
#ifndef __DERIVATION_H__
#define __DERIVATION_H__

#include <set>
#include <string>
#include <vector>

class rule;
class hypothesis;

class derivation {
public:
    typedef unsigned int size_type;

    derivation(const hypothesis* h);
    derivation(const derivation* d, const hypothesis* s, const hypothesis* r);
    ~derivation();

    void generate_hypothesis();
    double get_score() const;
    const hypothesis* get_hypothesis() const;
    const hypothesis* get_generated_hypothesis() const;
    void output(std::vector<const std::string*>& s);
    unsigned int sub_derivation_number() const;
    const derivation* get_sub_derivation(unsigned int i) const;
private:
    void initialize(const hypothesis* h);

    double score;
    const hypothesis* hypo;
    hypothesis* generated_hypothesis;
    std::vector<derivation*>* back_pointer;
};

struct derivation_less {
    bool operator()(const derivation* d1, const derivation* d2)
    {
        return d1->get_score() < d2->get_score();
    }
};

#include <iostream>

class derivation_queue {
public:
    derivation_queue(unsigned int capacity)
    {
        this->capacity = capacity;
    }
    ~derivation_queue()
    {

    }

    void push(derivation* d)
    {
        if (queue.size() < capacity) {
            queue.insert(d);
        } else {
            std::set<derivation*, derivation_less>::iterator iter;

            if (d->get_score() > (*queue.rbegin())->get_score()) {
                iter = queue.end();
                --iter;
                //delete *--iter;
                queue.erase(iter);
                queue.insert(d);
            } else {
            }
        }
    }

    derivation* pop()
    {
        std::set<derivation*, derivation_less>::iterator iter;
        iter = queue.begin();
        const derivation* d = *iter;
        queue.erase(iter);
        return const_cast<derivation*>(d);
    }

    bool empty()
    {
        return queue.empty();
    }

    unsigned int size()
    {
        return queue.size();
    }
private:
    std::multiset<derivation*, derivation_less> queue;
    unsigned int capacity;
};

#endif /* __DERIVATION_H__ */
