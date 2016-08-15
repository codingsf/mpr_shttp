#include "net/client.h"
#include "net/ip_util.h"
#include "net/socket_util.h"

#include <unistd.h>

#include <glog/logging.h>

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

  if (!SocketUtil::SetSocketDefaults(fd)) {
    ::close(fd);
    HandleConnect(CONNECT_ERROR);
    return;
  }

  Endpoint* endpoint = new Endpoint(options_.socket_family() != AF_INET);
  endpoint->set_fd(fd);
  ::memset(reinterpret_cast<char *>(endpoint->addr()), 0, endpoint->addrlen());
  if (options_.socket_family() == AF_INET) {
    struct sockaddr_in* addr = 
	    reinterpret_cast<struct sockaddr_in *>(endpoint->addr());
    addr->sin_family = AF_INET;
    addr->sin_port = ::htons(options_.port());
    struct sockaddr_in t;
    int err = IpUtil::GetAddressInfo(t, options_.host().c_str(),
		                     AF_INET,
				     SOCK_STREAM);
    if (err) {
      close(fd);
      delete endpoint;
      HandleConnect(CONNECT_ERROR);
      return;
    }
    ::memcpy(&(addr->sin_addr), &(t.sin_addr), sizeof(addr->sin_addr));
  } else { // UnixDomain
    struct sockaddr_un* addr = 
	    reinterpret_cast<struct sockaddr_un *>(endpoint->addr());
    addr->sun_family = options_.socket_family();
    snprintf(addr->sun_path, 
             sizeof(addr->sun_path), "%s", options_.sin_path().c_str());
  }

  errno = 0;
  if (::connect(endpoint->fd(),
                endpoint->addr(),
		endpoint->addrlen()) == 0 ||
      errno == EINPROGRESS) {
    auto cb = [endpoint, this] (EventLoop::Event event) {
      this->OnConnect(endpoint, event);
    };
    DCHECK(event_loop_->RegisterForWrite(endpoint->fd(), 
			                   std::move(cb),
					   false));
    return;
  } else {
    ::close(endpoint->fd());
    delete endpoint;
    HandleConnect(CONNECT_ERROR);
    return;
  }		  
}

void Client::Stop() {
  if (state_ == DISCONNECTED || state_ == CONNECTING) {
    return;
  }
  state_ = DISCONNECTED;
  conn_->Close();
}

void Client::OnConnect(Endpoint* endpoint, EventLoop::Event event) {
  int error = 0;
  socklen_t len = sizeof(error);

  if (event != EventLoop::WRITE_EVENT ||
      ::getsockopt(endpoint->fd(),
	           SOL_SOCKET,
		   SO_ERROR,
		   reinterpret_cast<void*>(&error), &len) < 0 ||
      error != 0) {
    close(endpoint->fd());
    delete endpoint;
    state_ = DISCONNECTED;
    HandleConnect(CONNECT_ERROR);
    return;
  }

  conn_ = CreateConnection(endpoint, &connection_options_, event_loop_);
  if (!conn_->Start()) {
    close(endpoint->fd());
    delete conn_;
    conn_ = nullptr;
    state_ = DISCONNECTED;
    HandleConnect(CONNECT_ERROR);
    return;
  }

  state_ = CONNECTED;
  conn_->RegisterForClose([this](NetworkErrorCode code) {
    this->OnClose(code);		  
  });
  HandleConnect(OK);
  return;
}

void Client::OnClose(NetworkErrorCode code) {
  delete conn_;
  conn_ = nullptr;
  state_ = DISCONNECTED;
  HandleClose(code);
}

int64_t Client::AddTimer(VCallback<> callback, int64_t msecs) {
  auto event_cb = [callback, this] (EventLoop::Event event) {
    this->OnTimer(std::move(callback), event);
  };
  int64_t timer_id = event_loop_->RegisterTimer(std::move(event_cb),
		                                false,
						msecs);
  CHECK_GT(timer_id, 0);
  return timer_id;
}

int Client::RemoveTimer(int64_t timer_id) {
  return event_loop_->UnregisterTimer(timer_id);
}

void Client::OnTimer(VCallback<> callback, EventLoop::Event) {
  callback();
}

} // namespace net
