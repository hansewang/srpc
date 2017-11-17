#include <google/protobuf/service.h>
#include <google/protobuf/descriptor.h>
#include <google/protobuf/stubs/common.h>
#include <google/protobuf/message.h>
#include "method_manager.h"
#include "dispatcher.h"
#include "connection.h"
#include <iostream>

namespace srpc
{
  MethodManager::MethodManager() {
  }

  MethodManager::~MethodManager() {
  }

  void MethodManager::RegisterService(gpb::Service *service) {
    const gpb::ServiceDescriptor *descriptor = service->GetDescriptor();
    const gpb::MethodDescriptor *method = descriptor->method(0);
    google::protobuf::Message *request = (google::protobuf::Message *)&service->GetRequestPrototype(method);
    google::protobuf::Message *response = (google::protobuf::Message *)&service->GetResponsePrototype(method);
    rpc_method_ = new RpcMethod(service, request, response, method);
    std::cout << "register service: " << method->full_name() << std::endl;
  }
  struct HandleServiceEntry {
    HandleServiceEntry(const gpb::MethodDescriptor *method, gpb::Message *request, gpb::Message *response)
      : method_(method), request_(request), response_(response) {
    }
    const gpb::MethodDescriptor *method_;
    google::protobuf::Message *request_;
    google::protobuf::Message *response_;
  };
  static void HandleServiceDone(HandleServiceEntry *entry, Connection *connection) {
    //static int num = 1;
    //std::cout << "HandleServiceDone num " << num++ << std::endl;
    connection->SetContent(entry->response_);
    delete entry->request_;
    delete entry->response_;
    delete entry;
  }
  bool MethodManager::HandlePacket(const char* buffer, uint32 length) {
    const gpb::MethodDescriptor *method = rpc_method_->method_;
    gpb::Message *request = rpc_method_->request_->New();
    gpb::Message *response = rpc_method_->response_->New();

    bool result = request->ParseFromArray(buffer, length);
    if (!result) {
      std::cout << "MethodManager:HandlePacket request err!!!" << std::endl;
    }

    HandleServiceEntry *entry = new HandleServiceEntry(method, request, response);
    gpb::Closure *done = gpb::NewCallback(&HandleServiceDone, entry, connection_);
    rpc_method_->service_->CallMethod(method,
      NULL,
      request, response, done);
    return true;
  }

}
