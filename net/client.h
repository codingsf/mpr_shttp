#ifndef NET_CLIENT_H_
#define NET_CLIENT_H_

#include <functional>
#include "net/connection.h"
#include "net/event_loop.h"
#include "net/network_options.h"
#include "net/network_error.h"

namespace net {


class Client {
 public:
  enum State { DISCONNECTED=0, CONNECTING, CONNECTED };
  Client(EventLoop* event_loop, const NetworkOptions& options);
  virtual ~Client();

  void Start();
  void Stop();
  int64_t AddTimer(VCallback<> cb, int64_t msecs);
  int RemoveTimer(int64_t timer_id);

  const NetworkOptions& connection_options() const { return options_; }

 protected:

  virtual Connection* CreateConnection(Endpoint* endpoint,
                                       Connection::Options* options,
                                       EventLoop* event_loop) = 0;
  virtual void HandleConnect(NetworkErrorCode code) = 0;
  virtual void HandleClose(NetworkErrorCode code) = 0;
  
  NetworkOptions options_;
  Connection* conn_;
  State state_;
  EventLoop* event_loop_;

 private:
  void Init(EventLoop* event_loop, const NetworkOptions& options);
  void OnConnect(Endpoint* endpoint, EventLoop::Event event);
  void OnClose(NetworkErrorCode code);
  void OnTimer(VCallback<> cb, EventLoop::Event event);

  Connection::Options connection_options_;
};

} // namespace net
#endif // NET_CLIENT_H_
