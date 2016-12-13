/* shared_mutex.cpp */
#include <mutex>
#include <shared_mutex.h>

shared_mutex::shared_mutex() : state_(0)
{
}

shared_mutex::~shared_mutex()
{
    std::lock_guard<std::mutex> _(mut_);
}

void shared_mutex::lock()
{
    std::unique_lock<std::mutex> lk(mut_);
    while (state_ & write_entered_)
        gate1_.wait(lk);
    state_ |= write_entered_;
    while (state_ & n_readers_)
        gate2_.wait(lk);
}

bool shared_mutex::try_lock()
{
    std::unique_lock<std::mutex> lk(mut_);

    if (state_ == 0) {
        state_ = write_entered_;
        return true;
    }
    return false;
}

void shared_mutex::unlock()
{
    std::lock_guard<std::mutex> _(mut_);
    state_ = 0;
    gate1_.notify_all();
}

void shared_mutex::lock_shared()
{
    std::unique_lock<std::mutex> lk(mut_);
    while ((state_ & write_entered_) || (state_ & n_readers_) == n_readers_)
        gate1_.wait(lk);
    unsigned int num_readers = (state_ & n_readers_) + 1;
    state_ &= ~n_readers_;
    state_ |= num_readers;
}

bool shared_mutex::try_lock_shared()
{
    std::unique_lock<std::mutex> lk(mut_);
    unsigned int num_readers = state_ & n_readers_;

    if (!(state_ & write_entered_) && num_readers != n_readers_) {
        ++num_readers;
        state_ &= ~n_readers_;
        state_ |= num_readers;
        return true;
    }

    return false;
}

void shared_mutex::unlock_shared()
{
    std::lock_guard<std::mutex> _(mut_);
    unsigned int num_readers = (state_ & n_readers_) - 1;
    state_ &= ~n_readers_;
    state_ |= num_readers;

    if (state_ & write_entered_) {
        if (num_readers == 0)
            gate2_.notify_one();
    } else {
        if (num_readers == n_readers_ - 1)
            gate1_.notify_one();
    }
}
