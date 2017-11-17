#ifndef __SRPC_DISPATCHER_H__
#define __SRPC_DISPATCHER_H__

#include <map>
#include "base.h"
#include "thread.h"

namespace srpc {

  static const uint32 kMaxPollWaitTime = 10;
  static const uint32 kEpollFdNum = 1024;

  enum EventFlags {
    EVENT_READ = 0x1,
    EVENT_WRITE = 0x1 << 1,
    EVENT_NULL
  };

  class EventHandler {
  public:
    virtual int HandleRead() = 0;
    virtual int HandleWrite() = 0;

    EventHandler() {
    }
    virtual ~EventHandler() {
    };
  };
  //class ThreadHandle;
  //class Thread;
  class Dispatcher : public ThreadHandle {
  public:
    Dispatcher();

    ~Dispatcher();

    int Start();

    void Stop();

    void Run();

    int AddEvent(int fd, uint32 flags, EventHandler *handler);   

  private:
    int epfd_;
    std::map<int, EventHandler*> event_handle_;
    Thread *thread_;
  };

  
};
#endif  //__SRPC_DISPATCHER_H__
