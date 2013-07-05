/*
 * trellis.h
 *
 *
 * author: Playinf
 * email: playinf@stu.xmu.edu.cn
 *
 */
#ifndef __TRELLIS_H__
#define __TRELLIS_H__

#include <memory>
#include <string>
#include <vector>
#include <hypothesis.h>

class trellis_node;
class trellis_path;
class trellis_detour;

class trellis_node {
public:
    typedef unsigned int size_type;

    trellis_node(const hypothesis* hypo);
    trellis_node(const trellis_detour* detour, trellis_node** deviation_node);
    ~trellis_node();

    size_type get_children_num() const;
    const hypothesis* get_hypothesis() const;
    const trellis_node* get_child(size_type index) const;

    void output(std::vector<const std::string*>* sentence) const;
private:
    trellis_node(const trellis_node* root, const trellis_node* substituted,
        const hypothesis* replacement, trellis_node** deviation_node);
    void create_children();
    void create_children(const trellis_node* root,
        const trellis_node* substituted, const hypothesis* replacement,
        trellis_node** deviation_node);

    const hypothesis* hypo;
    std::vector<trellis_node*> children;
};

class trellis_path {
public:
    trellis_path(const hypothesis* hypo);
    trellis_path(const trellis_detour* detour);
    ~trellis_path();

    float get_total_score() const;
    float get_heuristic_score() const;
    const trellis_node* get_final_node() const;
    const trellis_node* get_deviation_node() const;

    const std::vector<float>* get_score_vector() const;

    void output(std::vector<const std::string*>* sentence) const;
private:
    trellis_node* final_node;
    trellis_node* deviation_node;
    std::vector<float> score_vector;
    float heuristic_score;
    float total_score;
};

class trellis_detour {
public:
    trellis_detour(std::shared_ptr<const trellis_path> base_path,
        const trellis_node* substituted, const hypothesis* replacement);
    ~trellis_detour();

    float get_total_score() const;
    const trellis_path* get_base_path() const;
    const trellis_node* get_substituted_node() const;
    const hypothesis* get_replacement_hypothesis() const;
private:
    std::shared_ptr<const trellis_path> base_path;
    const trellis_node* substituted_node;
    const hypothesis* replacement_hypothesis;
    float total_score;
};

struct trellis_detour_less {
    bool operator()(const trellis_detour* d1, const trellis_detour* d2)
    {
        return d1->get_total_score() < d2->get_total_score();
    }
};

#endif /* __TRELLIS_H__ */
