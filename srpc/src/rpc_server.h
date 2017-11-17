#ifndef __SRPC_SERVER_H__
#define __SRPC_SERVER_H__

#include <string>
#include <google/protobuf/service.h>
#include "base.h"

namespace srpc
{
  class EventHandler;
  class Dispatcher;  
  class MethodManager;
  class RpcServer : public EventHandler {
  public:
    RpcServer(const char * ip, uint16 port);
    ~RpcServer();

    int Start(gpb::Service *service);
    int HandleRead();
    int HandleAccept();  
    int HandleWrite() {};

    void set_dispatcher(Dispatcher *dispatcher) {
      dispatcher_ = dispatcher;
    }
    void set_method_manager(MethodManager *method_manager) {
      method_manager_ = method_manager;
    }

  private:
    std::string listen_ip_;
    uint16 listen_port_;
    int listen_fd_;
    Dispatcher *dispatcher_;
    MethodManager *method_manager_;
  };
}
#endif //__SRPC_SERVER_H__