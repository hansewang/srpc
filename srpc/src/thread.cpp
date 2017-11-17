#include "thread.h"

namespace srpc {
  Thread::Thread(ThreadHandle *thread_handle)
    : thread_handle_(thread_handle) {

  }

  Thread::~Thread() {
  }

  void Thread::Start() {
    pthread_attr_t thread_attr;

    if (pthread_attr_init(&thread_attr) != 0) {
      return;
    }

    if (pthread_create(&pthread_, &thread_attr, ThreadProc, this) != 0) {
      return;
    }
  }

  void* Thread::ThreadProc(void* param) {
    Thread *thread = static_cast<Thread*>(param);
    thread->thread_handle_->Run();
  }
};
