#include "dispatcher.h"
#include "channel.h"
#include "base.h"


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
#include <string>
#include <iostream>

//#include "srpc.pb.h"

namespace srpc {

  Channel::Channel(const char *ip, uint16 port):ip_(ip), port_(port){
    fd_ = Connect();
    dispatcher_ = new Dispatcher();
    dispatcher_->Start();
    dispatcher_->AddEvent(fd_, EVENT_READ | EVENT_WRITE, this);
  }

  Channel::~Channel() {

  }

  bool Channel::SetNonBlocking(int &fd) {
    int flags = fcntl(fd, F_GETFL, 0);
    if (flags == -1) {
      return false;
    }
    if (-1 == fcntl(fd, F_SETFL, flags | O_NONBLOCK)) {
      return false;
    }
    return true;
  }

  bool Channel::is_connected(int fd, const fd_set *read_events, const fd_set *write_events) {
    int error_save = 0;
    socklen_t length = sizeof(error_save);
    // assume no error
    errno = 0;
    if (!FD_ISSET(fd, read_events) &&
      !FD_ISSET(fd, write_events)) {
      return false;
    }
    if (getsockopt(fd, SOL_SOCKET, SO_ERROR, &error_save, &length) < 0) {
      return false;
    }
    errno = error_save;
    return (error_save == 0);
  }
  int Channel::Connect() {
    struct sockaddr_in address;
    address.sin_family = AF_INET;
    inet_pton(AF_INET, ip_.c_str(), &(address.sin_addr));
    address.sin_port = htons(port_);
    int fd = ::socket(AF_INET, SOCK_STREAM, 0);
    if (fd < 0) {
      std::cout << "create socket error: " << strerror(errno) << std::endl;
      return -1;
    }
    if (!SetNonBlocking(fd)) {
      std::cout << "SetNonBlocking fail" << std::endl;
      ::close(fd);
      return -1;
    }
    const struct sockaddr_in *addr = &address;
    int ret = ::connect(fd, (struct sockaddr *)(addr), sizeof(*addr));
    if (ret == 0) {
      // connected?
      return fd;
    }
    // time-out connect
    fd_set read_events, write_events, exception_events;
    struct timeval tv;
    FD_ZERO(&read_events);
    FD_SET(fd, &read_events);
    write_events = read_events;
    exception_events = read_events;
    tv.tv_sec = 5;
    tv.tv_usec = 0;
    int result = ::select(fd + 1, &read_events,
      &write_events, &exception_events,
      &tv);
    if (result < 0) {
      std::cout << "select fail: " << strerror(errno) << std::endl;
      ::close(fd);
      return -1;
    }
    if (result == 0) {
      std::cout << "connect time out" << std::endl;
      ::close(fd);
      return -1;
    }
    if (is_connected(fd, &read_events, &write_events)) {
      std::cout << "connect to " /*<< address.DebugString()*/
        << " success" << std::endl;      
      return fd;
    }
    std::cout << "connect time out" << std::endl;
    ::close(fd);
    return -1;
  }

  void Channel::CallMethod(const srpc::gpb::MethodDescriptor* method,
    srpc::gpb::RpcController* controller,
    const srpc::gpb::Message* request,
    srpc::gpb::Message* response,
    srpc::gpb::Closure* done)
  {
    SetContent(request);
    set_message_response(response, done);
  }


  bool Channel::HandlePacket(const char* buffer, uint32 length) {
    bool result = message_response()->response->ParseFromArray(buffer, length);
    if (!result) {
      std::cout << "Channel:HandlePacket response err!!!" << std::endl;
    }
    //srpc::UnifiedReq response = 
    //std::cout << "Channel:HandlePacket response cmd=" << ((srpc::UnifiedReq*)(message_response()->response))->cmd() << std::endl;
    message_response()->done->Run();
  }

  int Channel::HandleRead() {
    for (; ;) {
      size_t result_len = 0;
      char buf[4096] = { 0 };

      result_len = read(fd_, buf, sizeof(buf) / sizeof(buf[0]));

      if (-1 == result_len) {
        if (EAGAIN != errno) {
          perror("Read data");
        }
        break;
      }
      else if (!result_len) {
        break;
      }

      //printf("write result_len %d\n", result_len);

      HandlePacket(buf, (uint32)result_len);   
        
    }
  }

  int Channel::HandleWrite() {
    std::string content;
    if (GetContent(content)) {
      write(fd_, content.c_str(), content.length());      
    }
  }

}