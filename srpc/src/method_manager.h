#ifndef __SRPC_METHOD_MANAGER_H__
#define __SRPC_METHOD_MANAGER_H__

#include "base.h"
#include <google/protobuf/service.h>

namespace srpc
{
  struct RpcMethod {
  public:
    RpcMethod(gpb::Service *service,
      gpb::Message *request,
      gpb::Message *response,
      const gpb::MethodDescriptor *method)
      : service_(service),
      request_(request),
      response_(response),
      method_(method) {
    }

    gpb::Service *service_;
    google::protobuf::Message *request_;
    google::protobuf::Message *response_;
    const gpb::MethodDescriptor *method_;
  };

  class Connection;
  class MethodManager {
  public:
    MethodManager();

    ~MethodManager();

    void RegisterService(gpb::Service *service);

    bool HandlePacket(const char* buffer, uint32 length);

    Connection *connection_;
  private:
    RpcMethod *rpc_method_;
   

  };

}
#endif //__SRPC_METHOD_MANAGER_H__