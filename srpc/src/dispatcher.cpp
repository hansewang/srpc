#include "thread.h"
#include "dispatcher.h"
#include <netinet/in.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/epoll.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>

//#include "method_manager.h"


namespace srpc {

  Dispatcher::Dispatcher() {

  }

  Dispatcher::~Dispatcher() {

  }

  int Dispatcher::Start() {
    epfd_ = epoll_create(kEpollFdNum);
    if (1 == epfd_) {
      perror("Create epoll instance");
      return 0;
    }
    thread_ = new Thread(this);
    thread_->Start();
  }

  int Dispatcher::AddEvent(int fd, uint32 flags, EventHandler *handler) {
    struct epoll_event ev;
    ev.data.fd = fd;
    ev.events = EPOLLIN | EPOLLET;
    int result = epoll_ctl(epfd_, EPOLL_CTL_ADD, fd, &ev);
    event_handle_[fd] = handler;
  }

  void Dispatcher::Run() {
    struct epoll_event event_array_[kEpollFdNum];
    for (; ; ) {
      int wait_count;
      wait_count = epoll_wait(epfd_, event_array_, kEpollFdNum, 1);
      for (int i = 0; i < wait_count; i++) {
        uint32_t events = event_array_[i].events;
        int fd = event_array_[i].data.fd;
        EventHandler *handle = event_handle_[fd];
     
        if (events & EPOLLIN) {
          handle->HandleRead();
        }        
        if (events & EPOLLOUT) {
          handle->HandleWrite();
        }
      }
      std::map<int, EventHandler*>::iterator it = event_handle_.begin();
      for (; it != event_handle_.end(); it++) {
        it->second->HandleWrite();
      }

    }
  }
  
  void Dispatcher::Stop() {

  }

}

