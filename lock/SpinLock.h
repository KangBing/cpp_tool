#pragma once
#include <pthread.h>
#include <assert.h>

namespace kang{


class SpinLock{
public:
    SpinLock(){
        assert(0 == pthread_spin_init(&spin_lock_, 0));
    }
    ~SpinLock(){
        pthread_spin_destroy(&spin_lock_);
    }
    void Lock(){
        assert(0 == pthread_spin_lock(&spin_lock_));
    }
    void UnLock(){
        assert(0 == pthread_spin_unlock(&spin_lock_));
    }
    void lock(){Lock();}
    void unlock(){UnLock();}

private:
    pthread_spinlock_t spin_lock_;
    
};

}

