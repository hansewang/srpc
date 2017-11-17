#include "dispatcher.h"
#include "rpc_server.h"
#include "connection.h"
#include "method_manager.h"

#include <netinet/in.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/epoll.h>
#include<stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <sys/select.h>

namespace srpc
{
  RpcServer::RpcServer(const char * ip, uint16 port) :
    listen_ip_(ip), listen_port_(port) {

  }

  RpcServer::~RpcServer() {

  }

  int RpcServer::Start(gpb::Service *service) {
    dispatcher_ = new Dispatcher();
    dispatcher_->Start();

    method_manager_ = new MethodManager();
    method_manager_->RegisterService(service);

    struct sockaddr_in server_addr = { 0 };
    listen_fd_ = socket(AF_INET, SOCK_STREAM, 0);
    if (-1 == listen_fd_) {
      perror("Open listen socket");
      return -1;
    }
    int on = 1;
    int result = setsockopt(listen_fd_, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));
    if (-1 == result) {
      perror("Set socket");
      return 0;
    }
    server_addr.sin_family = AF_INET;
    inet_aton(listen_ip_.c_str(), &(server_addr.sin_addr));
    server_addr.sin_port = htons(listen_port_);
    result = bind(listen_fd_, (const struct sockaddr *)&server_addr, sizeof(server_addr));
    if (-1 == result) {
      perror("Bind port");
      return 0;
    }
    int flags = fcntl(listen_fd_, F_GETFL, 0);
    if (flags == -1) {
      return -1;
    }
    if (-1 == fcntl(listen_fd_, F_SETFL, flags | O_NONBLOCK)) {
      return -1;
    }
    result = listen(listen_fd_, 200);
    if (-1 == result) {
      perror("Start listen");
      return 0;
    }

    dispatcher_->AddEvent(listen_fd_, EVENT_READ | EVENT_WRITE, this);

    printf("RpcServer::Start success\n");

    dispatcher_->Run();

    return 0;
  }

  int RpcServer::HandleRead() {
    HandleAccept();
  }

  int RpcServer::HandleAccept() {
    char host_buf[NI_MAXHOST];
    char port_buf[NI_MAXSERV];
    for (; ; ) { // ET
      struct sockaddr in_addr = { 0 };
      socklen_t in_addr_len = sizeof(in_addr);
      int accp_fd = accept(listen_fd_, &in_addr, &in_addr_len);
      if (-1 == accp_fd) {
        perror("Accept");
        break;
      }
      int result = getnameinfo(&in_addr, sizeof(in_addr),
        host_buf, sizeof(host_buf) / sizeof(host_buf[0]),
        port_buf, sizeof(port_buf) / sizeof(port_buf[0]),
        NI_NUMERICHOST | NI_NUMERICSERV);

      if (!result) {
        printf("New connection: host = %s, port = %s\n", host_buf, port_buf);
      }
      int flags = fcntl(accp_fd, F_GETFL, 0);
      if (flags == -1) {
        return -1;
      }
      if (-1 == fcntl(accp_fd, F_SETFL, flags | O_NONBLOCK)) {
        return -1;
      }

      srpc::Connection *connection = new srpc::Connection(accp_fd, method_manager_);

      dispatcher_->AddEvent(accp_fd, EVENT_READ | EVENT_WRITE, connection);

      //if (-1 == result) {
      //  perror("epoll_ctl");
      //  return 0;
      //}
    }
  }
}
