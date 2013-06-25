/*
 * shared_mutex.h
 *
 *
 * author: Playinf
 * email: playinf@stu.xmu.edu.cn
 *
 */
 #ifndef __SHARED_MUTEX_H__
 #define __SHARED_MUTEX_H__

 #include <mutex>
 #include <condition_variable>

class shared_mutex {
public:
    shared_mutex();
    ~shared_mutex();
    void lock();
    bool try_lock();
    void unlock();
    void lock_shared();
    bool try_lock_shared();
    void unlock_shared();

private:
    std::mutex mut_;
    std::condition_variable gate1_;
    std::condition_variable gate2_;
    unsigned int state_;

    static const unsigned write_entered_ = 1U << (sizeof(unsigned)* 8 - 1);
    static const unsigned n_readers_ = ~write_entered_;
};

 #endif /* __SHARED_MUTEX_H__ */
