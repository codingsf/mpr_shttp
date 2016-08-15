#ifndef NET_EVENT_LOOP_H_
#define NET_EVENT_LOOP_H_
#include <functional>
#include <unordered_map>

struct event_base;

namespace net {

template<typename... Args>
using VCallback = std::function<void(Args...)>;

class EventLoop {
 public:
  enum Event {
    UNKNOWN_EVENT = 0,
    READ_EVENT,
    WRITE_EVENT,
    SIGNAL_EVENT,
    TIMEOUT_EVENT,
    SYSTEM_ERROR_EVENT,
    NUM_EVENT_TYPES
  };

  EventLoop() {}
  virtual ~EventLoop() {}

  virtual void Loop() = 0;
  virtual int LoopExit() = 0;

  virtual bool RegisterForRead(int fd, VCallback<Event> cb, bool pers, 
                              int64_t timeout_micro_secs) = 0; 
  virtual bool RegisterForRead(int fd, VCallback<Event> cb, bool pers) = 0;
  virtual bool UnregisterForRead(int fd) = 0;

  virtual bool RegisterForWrite(int fd, VCallback<Event> cb, bool pers, 
                               int64_t timeout_micro_secs)=0;
  virtual bool RegisterForWrite(int fd, VCallback<Event> cb, bool pers) = 0;
  virtual bool UnregisterForWirte(int fd) = 0;

  virtual int64_t RegisterTimer(VCallback<Event> cb, bool pers, int64_t micro_secs) = 0;
  virtual bool UnregisterTimer(int64_t timer_id) = 0;
  virtual void RegisterInstantCallback(VCallback<> cb) = 0;
  virtual struct event_base* Dispatcher() = 0;
};

} // namespace net
#endif // NET_EVENT_LOOP_H_
