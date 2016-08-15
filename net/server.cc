#include "net/server.h"
#include "net/socket_util.h"

#include <glog/logging.h>

namespace net {

// Friend
void CallHandleConnectionCloseAndDelete(Server* server,
		                        Connection* connection,
					NetworkErrorCode code) {
  server->HandleConnectionClose(connection, code);
  delete connection;
}

Server::Server(EventLoop* event_loop,
               const NetworkOptions& options) {
  Init(event_loop, options);
}

Server::~Server() {}


bool Server::Start() {
  errno = 0;
  listen_fd_ = ::socket(options_.socket_family(), SOCK_STREAM, 0);
  if (listen_fd_ < 0) {
    return false;
  }

  if (!SocketUtil::SetSocketDefaults(listen_fd_)) {
    ::close(listen_fd_);
    return false; 
  }

  struct sockaddr_in in_addr;
  struct sockaddr_un unix_addr;
  struct sockaddr* server_addr = nullptr;
  socklen_t sockaddr_len = 0;
  if (options_.socket_family() == AF_INET) {
    memset(reinterpret_cast<char*>(&in_addr), 0, sizeof(in_addr));
    in_addr.sin_family = options_.socket_family();
    in_addr.sin_port = ::htons(options_.port());
    in_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr = (struct sockaddr *)&in_addr;
    sockaddr_len = sizeof(in_addr);
  } else {
    memset(reinterpret_cast<char*>(&unix_addr), 0, sizeof(unix_addr));
    unix_addr.sun_family = options_.socket_family();
    snprintf(unix_addr.sun_path, sizeof(unix_addr.sun_path), "%s",
             options_.sin_path().c_str());
    server_addr = (struct sockaddr *)&unix_addr;
    sockaddr_len = sizeof(unix_addr);
  }

  if (::bind(listen_fd_, server_addr, sockaddr_len) < 0) {
    ::close(listen_fd_);
    return false;
  }

  if (::listen(listen_fd_, 100) < 0) {
    ::close(listen_fd_);
    return false;
  }

  if (!event_loop_->RegisterForRead(listen_fd_, on_new_connection_callback_,
			           true)) {
    close(listen_fd_);
    return false;
  }
  return true;
}

bool Server::Stop() {
  DCHECK(event_loop_->UnregisterForRead(listen_fd_));
  ::close(listen_fd_);

  while (active_connections_.size() > 0) {
    Connection* conn = *(active_connections_.begin());
    conn->Close();
  }
  CHECK(active_connections_.empty());

  return true;
}

void Server::OnNewConnection(EventLoop::Event event) {
  if (event == EventLoop::READ_EVENT) {
    Endpoint* endpoint = new Endpoint(options_.socket_family() != AF_INET);
    struct sockaddr* server_addr = endpoint->addr();
    socklen_t addrlen = endpoint->addrlen();
    int fd = ::accept(listen_fd_, server_addr, &addrlen);
    endpoint->set_fd(fd);
    if (endpoint->fd() > 0) {
      if (!SocketUtil::SetSocketDefaults(endpoint->fd())) {
        ::close(endpoint->fd());
	delete endpoint;
	return;
      }

      Connection* conn = CreateConnection(endpoint,
		                          &connection_options_,
					  event_loop_);
      auto ccb = [conn, this] (NetworkErrorCode code) {
        this->OnConnectionClose(conn, code);
      };

      conn->RegisterForClose(std::move(ccb));

      if (!conn->Start()) {
        ::close(endpoint->fd());
	delete conn;
	return;
      }

      active_connections_.insert(conn);
      HandleNewConnection(conn);
      return;
    } else {
      if (errno == EAGAIN) {
      } else {
      }
      ::close(endpoint->fd());
      delete endpoint;
      return;
    }
  } else {
    return;
  }
}

void Server::OnConnectionClose(Connection* connection,
		               NetworkErrorCode code) {
  if (active_connections_.find(connection) == active_connections_.end()) {
    code = INVALID_CONNECTION;
  } else {
    active_connections_.erase(connection);
  }
  HandleConnectionClose(connection, code);
  delete connection;
}

void Server::CloseConnection(Connection* connection) {
  InternalCloseConnection(connection);
  return;
}

void Server::InternalCloseConnection(Connection* connection) {
  if (active_connections_.find(connection) == active_connections_.end()) {
    return;
  }
  connection->Close();
  return;
}

void Server::AddTimer(VCallback<> callback, int64_t msecs) {
  InternalAddTimer(std::move(callback), msecs);
}

void Server::InternalAddTimer(VCallback<> callback, int64_t msecs) {
  auto cb = [callback, this] (EventLoop::Event event) {
    this->OnTimer(std::move(callback), event);
  };
  DCHECK(event_loop_->RegisterTimer(cb, false, msecs));
}

void Server::OnTimer(VCallback<> callback, EventLoop::Event) {
  callback();
}

} // namespace net
