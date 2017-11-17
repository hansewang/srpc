#ifndef __SRPC_CONTENT_H__
#define __SRPC_CONTENT_H__

#include "base.h"
#include "mutex.h"
#include <string>
#include <google/protobuf/message.h>

namespace srpc {
  
  class ContentHandle : public MutexHandle {
  public:
    ContentHandle();
    ~ContentHandle();

    void SetContent(const gpb::Message *message);
    bool GetContent(std::string &content);
    void Clear();

  private:
    std::string content_;      
  };
};
#endif  // __SRPC_CONTENT_H__
