#ifndef __SRPC_MUTEX_H__
#define __SRPC_MUTEX_H__

#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>

namespace srpc {

  class MutexHandle {
  public:
    MutexHandle() {      
      if (pthread_mutex_init(&pthread_mutex_, NULL) != 0) {
        abort();
      }
      printf("MutexHandle Init\n");
    }

    ~MutexHandle() {
      if (pthread_mutex_destroy(&pthread_mutex_) != 0) {
        abort();
      }
      printf("MutexHandle unInit\n");
    }

    void Lock() const {
      pthread_mutex_lock(&pthread_mutex_);
    }

    bool Trylock() const {
      return (pthread_mutex_trylock(&pthread_mutex_) == 0);
    }

    void Unlock() const {
      pthread_mutex_unlock(&pthread_mutex_);
    }

  private:
    mutable pthread_mutex_t pthread_mutex_;    
  };

};

#endif  // __SRPC_MUTEX_H__
