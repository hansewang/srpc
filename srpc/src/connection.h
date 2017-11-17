#ifndef __SRPC_CONNECTION_H__
#define __SRPC_CONNECTION_H__
#include <string>
#include "base.h"
#include "content_handle.h"

namespace srpc {

  class EventHandler;
  class MethodManager;
  class Connection : public EventHandler, public ContentHandle {
  public:
    Connection(int fd, MethodManager *method_manager);
    ~Connection();

    virtual int HandleRead();
    virtual int HandleWrite();

    //std::string *send_data() {
    //  return &send_data_;
    //}

  private:
    int fd_;
    //std::string send_data_;    
    MethodManager *method_manager_;
  };
};
#endif  // __SRPC_CONNECTION_H__
