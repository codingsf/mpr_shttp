#ifndef NET_SERVER_H_
#define NET_SERVER_H_
#include <functional>
#include <unordered_set>
#include "net/connection.h"
#include "net/event_loop.h"
#include "net/network_options.h"
#include "net/network_error.h"

namespace std {

template<>
struct hash<net::Connection*> {
  size_t operator()(net::Connection* const& x) const {
    hash<void*> h;
    return h(reinterpret_cast<void*>(x));
  }
};

} // namespace std

namespace net {

class PCQueue;

class Server {
 public:
  Server(EventLoop* event_loop, const NetworkOptions& options);
  virtual ~Server();

  bool Start();
  bool Stop();
  void CloseConnection(Connection* connection);
  void AddTimer(VCallback<> callback, int64_t msecs);

  const NetworkOptions& server_options() const { return options_; }

  friend void CallHandleConnectionCloseAndDelete(Server*,
		                                 Connection*,
						 NetworkErrorCode);
 protected:
  virtual Connection* CreateConnection(Endpoint* endpoint,
		                       Connection::Options* options,
				       EventLoop* event_loop) = 0; 
  virtual void HandleNewConnection(Connection* new_connection) = 0;
  virtual void HandleConnectionClose(Connection* connection,
		                     NetworkErrorCode code) = 0;

  EventLoop* event_loop_;
  std::unordered_set<Connection*> active_connections_;

 private:
  void Init(EventLoop* event_loop, const NetworkOptions& options);
  void OnNewConnection(EventLoop::Event event);
  void OnConnectionClose(Connection* connection, NetworkErrorCode code);
  void OnTimer(VCallback<> callback, EventLoop::Event event);

  void InternalCloseConnection(Connection* connection);
  void InternalAddTimer(VCallback<> callback, int64_t msecs);

  int listen_fd_;
  Connection::Options connection_options_;
  NetworkOptions options_;
  VCallback<EventLoop::Event> on_new_connection_callback_;
};

} // namespace net
#endif // NET_SERVER_H_
