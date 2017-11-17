#include "dispatcher.h"
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



namespace srpc {

  Connection::Connection(int fd, MethodManager *method_manager) 
    : fd_(fd), method_manager_(method_manager){
    method_manager_->connection_ = this;
  }

  Connection::~Connection() {

  }

  int Connection::HandleRead() {
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
      method_manager_->HandlePacket(buf, (uint32)result_len);
      //printf("write result_len %d\n", result_len);
    }
  }

  int Connection::HandleWrite() {
    std::string content;
    if (GetContent(content)) {
      write(fd_, content.c_str(), content.length());
    }
  }

}