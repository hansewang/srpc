#include <google/protobuf/service.h>
#include <google/protobuf/descriptor.h>
#include <google/protobuf/stubs/common.h>
#include <google/protobuf/message.h>
#include <iostream>
#include "dispatcher.h"
#include "rpc_server.h"
#include "channel.h"
#include "method_manager.h"
#include "srpc.pb.h"

using namespace srpc;

class RpcServiceImpl : public srpc::GlobalService {
public:
  RpcServiceImpl() {
  };

  virtual void GlobalCall(::google::protobuf::RpcController* controller,
    const ::srpc::UnifiedReq* request,
    ::srpc::UnifiedReq* response,
    ::google::protobuf::Closure* done) {
    std::cout << "GlobalService request: " << request->cmd() << std::endl;
    static int num_resp = 10001;
    response->set_cmd(num_resp++);
    if (done) {
      done->Run();
    }
  }
};


void GlobalCall_response(srpc::UnifiedReq *response) {
  std::cout << "GlobalCall response: " << response->cmd() << std::endl;
}

int main(int argc, char *argv[])
{
  if (argc != 2 || (atoi(argv[1]) != 0 && atoi(argv[1]) != 1)) {
    std::cout << "argv[1] need 1(srv) or 0(client)" << std::endl;
    return 0;
  }
  std::cout << "argv " << argv[1] << std::endl;
  if (atoi(argv[1]) == 0) {         // client mode
    RpcController control;   
    Channel channel("127.0.0.1", 5566);
    srpc::GlobalService_Stub stub(&channel);
    int num_cmd = 1;
    while (1) {      
      srpc::UnifiedReq *response = new srpc::UnifiedReq;
      srpc::UnifiedReq *requst = new srpc::UnifiedReq;
      requst->set_cmd(num_cmd++);     
      std::cout << "GlobalCall request " << requst->cmd() << std::endl;
      stub.GlobalCall(&control, requst, response, srpc::gpb::NewCallback(GlobalCall_response, response));      
      sleep(1);
    }
  }
  else if (atoi(argv[1]) == 1) {    // server mode

    srpc::gpb::Service *service = new RpcServiceImpl();
    RpcServer *rpcsrv = new RpcServer("127.0.0.1", 5566);
    rpcsrv->Start(service);
    while (1) {
      sleep(10);
    }
  }

  return 0;
}
