#include "content_handle.h"
#include <stdio.h>

namespace srpc {

  ContentHandle::ContentHandle() {

  }

  ContentHandle::~ContentHandle() {

  }

  void ContentHandle::SetContent(const gpb::Message *message) {
    Lock();
    bool result = message->SerializeToString(&content_);
    if (!result) {
      printf("ContentHandle::SetContent err!!!");
    }
    Unlock();
  }

  bool ContentHandle::GetContent(std::string &content) {
    Lock();
    content.assign(content_);
    content_.clear();
    Unlock();
    return !content.empty();
  }

  void ContentHandle::Clear() {

  }

}