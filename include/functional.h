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

class rule;
class symbol;
class rt_node;

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
    size_t operator()(const rt_node& node) const;
};

#endif /* __FUNCTIONAL_H__ */
