#ifndef __SRPC_CHANNEL_H__
#define __SRPC_CHANNEL_H__

#include "base.h"
#include <string>
#include <google/protobuf/service.h>
#include <google/protobuf/descriptor.h>
#include <google/protobuf/stubs/common.h>
#include <google/protobuf/message.h>
#include "content_handle.h"

namespace srpc {

  class RpcController : public srpc::gpb::RpcController {
  public:
    RpcController() {};
    virtual ~RpcController() {};

    // client-end methods
    virtual void Reset() {};
    virtual bool Failed() const { return !m_fail_reason.empty(); };
    virtual std::string ErrorText() const { return m_fail_reason; };
    virtual void StartCancel() {};

    // server-end methods
    virtual void SetFailed(const std::string& reason) { m_fail_reason = reason; };
    virtual bool IsCanceled() const { return false; };
    virtual void NotifyOnCancel(srpc::gpb::Closure* callback) {};

  private:
    std::string m_fail_reason;
  };

  class Dispatcher;
  class EventHandler;
  class Channel : public srpc::gpb::RpcChannel, public EventHandler, public ContentHandle {
  public:
    Channel(const char *ip, uint16 port);
    ~Channel();

    struct MessageResponse {
      srpc::gpb::Message* response;
      srpc::gpb::Closure* done;
    };

    bool is_connected(int fd, const fd_set *read_events, const fd_set *write_events);
    bool SetNonBlocking(int &fd);
    int Connect();

    virtual int HandleRead();
    virtual int HandleWrite();

    void CallMethod(const srpc::gpb::MethodDescriptor* method,
      srpc::gpb::RpcController* controller,
      const srpc::gpb::Message* request,
      srpc::gpb::Message* response,
      srpc::gpb::Closure* done);

    bool HandlePacket(const char* buffer, uint32 length);

    MessageResponse *message_response() {
      return &message_response_;
    }

    void set_message_response(srpc::gpb::Message* response, srpc::gpb::Closure* done) {
      message_response_.response = response;
      message_response_.done = done;
    };

    //std::string *send_data() {
    //  return &send_data_;
    //}
   
  private:
    MessageResponse message_response_;
    //std::string send_data_;
    int fd_;
    Dispatcher *dispatcher_;
    std::string ip_;
    uint16 port_;
    //Content content_;
  };
};
#endif  // __SRPC_CHANNEL_H__
