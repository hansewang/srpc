#ifndef __SRPC_THREAD_H__
#define __SRPC_THREAD_H__

#include <pthread.h>

namespace srpc {
  class ThreadHandle {
  public:
    virtual ~ThreadHandle() {};
    virtual void Run() = 0;
  };

  class Thread {
  public:
    Thread(ThreadHandle *thread_handle);

    ~Thread();

    void Start();

  private:
    static void* ThreadProc(void* param);

  private:
    pthread_t pthread_;
    ThreadHandle *thread_handle_;
  };
};
#endif  // __SRPC_THREAD_H__
