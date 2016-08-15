#ifndef NET_REGISTERED_EVENT_H_
#define NET_REGISTERED_EVENT_H_
#include <event.h>
#include <sys/time.h>
#include <functional>
#include "base/time.h"

#include "net/event_loop.h"

namespace net {

template<typename T>
class RegisteredEvent {
 public:
  RegisteredEvent(const T& fd, bool persistent,
                  VCallback<EventLoop::Event> callback,
                  int64_t micro_secs)
    : fd_(fd),
      persistent_(persistent),
      callback_(std::move(callback)) {
    timer_.tv_sec = micro_secs / base::Time::kMicrosecondsPerSecond;
    timer_.tv_usec = micro_secs % base::Time::kMicrosecondsPerSecond;
  }
  
  ~RegisteredEvent() {}

  struct event* event() const {
    return &event_;
  }

  struct timerval* timer() const {
    return &timer_;
  }

  T fd() const {
    return fd_;
  }

  const VCallback<EventLoop::Event>& callback() const { return callback_; }

  bool IsPersistent() const {
    return persistent_;
  }
     
 private:
  struct event event_;
  T fd_;
  bool persistent_;
  VCallback<EventLoop::Event> callback_;
  //TODO
  //base::TimeDelta
  struct timeval timer_;
};

} // namespace net
#endif // NET_REGISTERED_EVENT_H_
