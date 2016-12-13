/*
 * information.h
 *
 * author: Playinf
 * email: playinf@stu.xmu.edu.cn
 *
 */
#ifndef __INFORMATINO_H__
#define __INFORMATION_H__

#include <vector>

class symbol;

class information {
public:
    information();
    ~information();

    information(const information&) = delete;
    const information& operator=(const information&) = delete;

    std::vector<const symbol*>* get_sentence() const;

    void set_sentence(std::vector<const symbol*>* sentence);
private:
    std::vector<const symbol*>* sentence;
};

#endif /* __INFORMATINO_H__ */
