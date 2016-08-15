#include "net/client.h"

namespace net {

Client::Client(EventLoop* event_loop,
               const NetworkOptions& options) {
  Init(event_loop, options);
}

void Client::Init(EventLoop* event_loop, const NetworkOptions& options) {
  event_loop_ = event_loop;
  options_ = options;
  conn_ = nullptr;
  connection_options_.max_packet_size = options_.max_packet_size();
  state_ = DISCONNECTED;
}

Client::~Client() {}

void Client::Start() {
  if (state_ != DISCONNECTED) {
    HandleConnect(DUPLICATE_START);
    return;
  }
  int fd = -1;
  fd = ::socket(options_.socket_family(),
                SOCK_STREAM,
                0);
  if (fd < 0) {
    HandleConnect(CONNECT_ERROR);
    return;
  }
  //TODO(wqx):
  // set socket options

  Endpoint* endpoint = new Endpoint(options_.socket_family() != AF_INET);
  endpoint->set_fd(fd);
  ::memset(reinterpret_cast<char *>(endpoint->addr()), 0, endpoint->addrlen());
  if (options_.socket_family() == AF_INET) {
    struct sockaddr_in* addr = reinterpret_cast<struct sockaddr_in *>(endpoint->addr());
    addr->sin_family = AF_INET;
    addr->sin_port = ::htons(options_.port());
    struct sockaddr_in t;
    int err = 
  } else { // UnixDomain
  }
}

} // namespace net
