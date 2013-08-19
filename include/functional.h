/*
 * functional.h
 *
 *
 * author: Playinf
 * email: playinf@stu.xmu.edu.cn
 *
 */
#ifndef __FUNCTIONAL_H__
#define __FUNCTIONAL_H__

class cube;
class rule;
class symbol;
class rt_node;
class alignment;
class cube_item;
class hypothesis;
class trellis_detour;

struct symbol_hash {
    std::size_t operator()(const symbol& sym) const;
};

struct symbol_address_hash {
    std::size_t operator()(const symbol* sym) const;
};

struct symbol_equal {
    std::size_t operator()(const symbol& s1, const symbol& s2) const;
};

struct rule_less {
    bool operator()(const rule* r1, const rule* r2) const;
};

struct rt_node_hash {
    std::size_t operator()(const rt_node& node) const;
};

struct cube_item_hash {
    std::size_t operator()(const cube_item* item) const;
};

struct cube_item_equal_to {
    bool operator()(const cube_item* item1, const cube_item* item2) const;
};

struct cube_item_less {
    bool operator()(const cube_item* item1, const cube_item* item2) const;
};

struct cube_less {
    bool operator()(const cube* c1, const cube* c2) const;
};

struct hypothesis_less {
    bool operator()(const hypothesis* h1, const hypothesis* h2) const;
};

struct hypothesis_compare {
    bool operator()(const hypothesis* h1, const hypothesis* h2) const;
};

struct trellis_detour_less {
    bool operator()(const trellis_detour* d1, const trellis_detour* d2) const;
};

struct alignment_hash {
    std::size_t operator()(const alignment& a) const;
};

struct alignment_equal {
    bool operator()(const alignment& a1, const alignment& a2) const;
};

#endif /* __FUNCTIONAL_H__ */
