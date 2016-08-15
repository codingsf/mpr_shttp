#include "net/connection.h"
#include <string>
#include <glog/logging.h>

namespace net {

Connection::Connection(Endpoint* endpoint,
                       Options* options,
                       EventLoop* event_loop) 
    : options_(options),
      endpoint_(endpoint),
      event_loop_(event_loop) {
  state_ = INIT;
  read_state_ = NOT_REGISTERED;
  write_state_ = NOT_REGISTERED;
  on_close_ = nullptr;
  on_read_ = [this] (EventLoop::Event ev) { return this->HandleRead(ev); };
  on_write_= [this] (EventLoop::Event ev) { return this->HandleWrite(ev); };
  can_close_connection_ = true;
}

Connection::~Connection() {
  CHECK(state_ == INIT || state_ == DISCONNECTED );
}

bool Connection::Start() {
  if (state_ != INIT) {
    return false;
  }
  if (RegisterEndpointForRead() < 0) {
    return false;
  }
  state_ = CONNECTED;
  return true;
}

void Connection::Close() {
  if (state_ != CONNECTED) {
    return;
  }
  state_ = DISCONNECTING;
  CloseInternal();
}

void Connection::CloseInternal() {
  if (state_ != DISCONNECTING) {
    return;
  }
  if (!can_close_connection_) {
    return;
  }
  state_ = DISCONNECTED;

  NetworkErrorCode code;
  if (read_state_ == ERROR) {
    code = READ_ERROR;
  } else if (write_state_ == ERROR) {
    code = WRITE_ERROR;
  } else {
    code = OK;
  }

  if (read_state_ != NOT_REGISTERED) {
    DCHECK(UnregisterEndpointForRead());
  }

  if (write_state_ == NOT_READY) {
    int write_unsubscribe = event_loop_->UnregisterForWirte(endpoint_->fd());
    CHECK_EQ(write_unsubscribe, 0);
  }
  write_state_ = NOT_REGISTERED;

  int retval = ::close(endpoint_->fd());
  
  if (retval != 0) {
    if (on_close_) {
      on_close_(CLOSE_ERROR);
    }
  } else {
    if (on_close_) {
      on_close_(code);
    }
  }
}

bool Connection::RegisterForWrite() {
  if (state_ != CONNECTED) {
    return false;
  }
  if (write_state_ == NOT_REGISTERED) {
    CHECK(event_loop_->RegisterForWrite(endpoint_->fd(), on_write_, false));
    write_state_ = NOT_READY;
  }
  return true;
}

void Connection::RegisterForClose(VCallback<NetworkErrorCode> on_close) {
  on_close_ = std::move(on_close);
}

void Connection::HandleWrite(EventLoop::Event event) {
  DCHECK_EQ(event, EventLoop::WRITE_EVENT);
  write_state_ = NOT_REGISTERED;

  if (state_ != CONNECTED) return;

  if (!WriteIntoEndPoint(endpoint_->fd())) {
    write_state_ = ERROR;
    state_ = DISCONNECTING;
  }  
  if (state_ == CONNECTED &&
      write_state_ == NOT_REGISTERED &&
      StillHaveDataToWrite()) {
    write_state_ = NOT_READY;
    DCHECK(event_loop_->RegisterForWrite(endpoint_->fd(), on_write_, false));
  }

  bool prev = can_close_connection_;
  can_close_connection_ = false;
  HandleDataWritten();
  can_close_connection_ = prev;
  if (state_ != CONNECTED) {
    CloseInternal();
  }
}

void Connection::HandleRead(EventLoop::Event event) {
  CHECK(event == EventLoop::READ_EVENT );
  read_state_ = READY;
  if (ReadFromEndPoint(endpoint_->fd())) {
    read_state_ = NOT_READY;
  } else {
    read_state_ = ERROR;
    state_ = DISCONNECTING;
  }

  bool prev = can_close_connection_;
  can_close_connection_ = false;
  HandleDataRead();
  can_close_connection_ = prev;
  if (state_ != CONNECTED) {
    CloseInternal();
  }
}

std::string Connection::ip_address() {
  std::string result;
  if (!endpoint_->IsUnixSocket()) {
    char addr_str[INET_ADDRSTRLEN];
    struct sockaddr_in* addr_in = (struct sockaddr_in*)(endpoint_->addr());
    if (::inet_ntop(addr_in->sin_family, &(addr_in->sin_addr), addr_str, INET_ADDRSTRLEN)) {
      result = addr_str;
    } else {
      result = "";
    }
  } else {
    struct sockaddr_un* addr_un = (struct sockaddr_un*)(endpoint_->addr());
    result = addr_un->sun_path;
  }
  return result;
}

int Connection::port() {
  if (!endpoint_->IsUnixSocket()) {
    return ::ntohs(((struct sockaddr_in*)(endpoint_->addr()))->sin_port);
  } else {
    return -1;
  }
}

bool Connection::UnregisterEndpointForRead() {
  if (!event_loop_->UnregisterForRead(endpoint_->fd())) {
    return false;
  }
  read_state_ = NOT_REGISTERED;
  return true;
}

bool Connection::RegisterEndpointForRead() {
  if (!event_loop_->RegisterForRead(endpoint_->fd(), on_read_, true)) {
    return false;
  }  
  read_state_ = NOT_READY;
  return true;
}

} // namespace net
