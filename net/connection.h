#ifndef NET_CONNECTION_H_
#define NET_CONNECTION_H_
#include <sys/un.h>
#include <arpa/inet.h>
#include <functional>

#include "net/event_loop.h"
#include "net/network_error.h"

namespace net {

class Endpoint {
 public:
  explicit Endpoint(bool unix_socket) {
    fd_ = -1;
    unix_socket_ = unix_socket;
  }

  struct sockaddr* addr() {
    if (unix_socket_) {
      return (struct sockaddr*)&un_addr_;
    } else {
      return (struct sockaddr*)&in_addr_;
    }
  }

  socklen_t addrlen() {
    if (unix_socket_) {
      return sizeof(un_addr_);
    } else {
      return sizeof(in_addr_);
    }
  }

  void set_fd(int fd) { fd_ = fd; }
  int fd() { return fd_; }
  bool IsUnixSocket() const { return unix_socket_; }

 private:
  int fd_;
  bool unix_socket_;
  struct sockaddr_in in_addr_;
  struct sockaddr_un un_addr_;
};

class Connection {
 public:
  struct Options {
    uint32_t max_packet_size;
  };
  enum State {
    INIT=0,
    CONNECTED,
    DISCONNECTED,
    DISCONNECTING,
  };
  enum ReadWriteState {
    NOT_REGISTERED,
    READY,
    NOT_READY,
    ERROR
  };
  
  Connection(Endpoint* endpoint, Options* options, EventLoop* event_loop);
  virtual ~Connection();

  bool Start();
  void Close();
  void RegisterForClose(VCallback<NetworkErrorCode> cb);
  std::string ip_address();
  int port();

 protected:
  virtual bool WriteIntoEndPoint(int fd) = 0;
  virtual bool StillHaveDataToWrite() = 0;
  virtual void HandleDataWritten() = 0;
  virtual bool ReadFromEndPoint(int fd) = 0;
  virtual void HandleDataRead() = 0;

  bool RegisterForWrite();
  int connection_fd() const;

  bool RegisterEndpointForRead();
  bool UnregisterEndpointForRead();

  Options* options_;

 private:
  void HandleRead(EventLoop::Event);
  void HandleWrite(EventLoop::Event);
  void CloseInternal();
  
  State state_;
  ReadWriteState read_state_;
  ReadWriteState write_state_;
  VCallback<NetworkErrorCode> on_close_;
  VCallback<EventLoop::Event> on_read_;
  VCallback<EventLoop::Event> on_write_;

  Endpoint* endpoint_;

  //
  EventLoop* event_loop_;
  bool can_close_connection_;
};

} // namespace net
#endif // NET_CONNECTION_H_
