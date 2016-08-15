#include "net/socket_util.h"
#include "base/file_util.h"
#include <fcntl.h>
#include <netinet/in.h>
#include <netinet/tcp.h>

#include <glog/logging.h>

namespace net {

namespace {

static const int kTcpKeepAliveSecs = 300; // 5 min
static const int kTcpKeepAliveProbeInterval = 10; // 10 seconds
static const int kTcpKeepAliveProbes = 5;
} // namespace

bool SocketUtil::SetNonBlocking(int fd) {
  return base::SetNonBlocking(fd);
}

bool SocketUtil::GetSendBufferSize(int fd, int& ret) {
  socklen_t optlen = sizeof(int);
  if (::getsockopt(fd, SOL_SOCKET, SO_SNDBUF, &ret, &optlen) != 0) {
    return false;
  }
  return true;
}

bool SocketUtil::SetSendBufferSize(int fd, int size) {
  if (::setsockopt(fd, SOL_SOCKET, SO_SNDBUF, 
	           reinterpret_cast<char *>(&size), sizeof(size)) != 0) {
    return false;
  }
  return true;
}

bool SocketUtil::GetRecvBufferSize(int fd, int& ret) {
  socklen_t optlen = sizeof(int);
  if (::getsockopt(fd, SOL_SOCKET, SO_RCVBUF, &ret, &optlen) != 0) {
    return false;
  }
  return true;
}

bool SocketUtil::SetRecvBufferSize(int fd, int size) {
  if (::setsockopt(fd, SOL_SOCKET, SO_RCVBUF, 
                   reinterpret_cast<char *>(&size),
		   sizeof(size)) != 0) {
    return false;
  }
  return true;
}

bool SocketUtil::SetReuseAddress(int fd) {
  int sopt = 1;
  if (::setsockopt(fd,
                   SOL_SOCKET,
		   SO_REUSEADDR,
		   reinterpret_cast<char *>(&sopt),
		   sizeof(sopt)) != 0) {
    return false;
  }
  return true;
}

bool SocketUtil::SetKeepAlive(int fd) {
  int alive = 1;
  if (::setsockopt(fd,
                   SOL_SOCKET,
		   SO_KEEPALIVE,
		   reinterpret_cast<char *>(&alive),
		   sizeof(alive)) != 0) {
    return false;
  }
  return true;
}

bool SocketUtil::SetKeepIdleTime(int fd, int time) {
  if (::setsockopt(fd,
                   SOL_TCP,
		   TCP_KEEPIDLE,
		   reinterpret_cast<char *>(&time),
		   sizeof(time)) != 0) {
    return false;
  }
  return true;
}

bool SocketUtil::SetKeepIdleCount(int fd, int count) {
  if (::setsockopt(fd,
                   SOL_TCP,
		   TCP_KEEPCNT,
		   reinterpret_cast<char *>(&count),
		   sizeof(count)) != 0) {
    return false;
  }
  return true;
}

bool SocketUtil::SetKeepIdleInterval(int fd, int interval) {
  if (::setsockopt(fd,
                   SOL_TCP,
		   TCP_KEEPINTVL,
		   reinterpret_cast<char *>(&interval),
		   sizeof(interval)) != 0) {
    return false;
  }
  return true;
}

bool SocketUtil::SetKeepIdleParams(int fd,
		                   int time,
				   int count,
				   int interval) {
  if (!SetKeepIdleTime(fd, time)) {
    return false;
  }
  if (!SetKeepIdleCount(fd, count)) {
    return false;
  }
  if (!SetKeepIdleInterval(fd, interval)) {
    return false;
  }
  return true;
}

bool SocketUtil::SetTCPNoDelay(int fd) {
  int on = 1;
  if (::setsockopt(fd, IPPROTO_TCP, TCP_NODELAY, reinterpret_cast<char *>(&on),
                   sizeof(on)) != 0) {
    return false;  
  }
  return true;
}

bool SocketUtil::SetSocketDefaults(int fd) {
  int send_buf;

  auto res = SocketUtil::GetSendBufferSize(fd, send_buf);
  if (!res) {
    //ERROR  
  } else {
    send_buf *= 100;
    res = SocketUtil::SetSendBufferSize(fd, send_buf);
    if (!res) {
      //ERROR
    }
  }

  int recv_buf;
  res = SocketUtil::GetRecvBufferSize(fd, recv_buf);
  if (!res) {
    //ERROR
  } else {
    recv_buf *= 100;
    res = SocketUtil::SetRecvBufferSize(fd, recv_buf);
    if (!res) {
    //ERROR
    }
  }

  if (!SocketUtil::SetNonBlocking(fd)) {
    return false;
  }

  if (!SocketUtil::SetKeepAlive(fd)) {
    return false;
  }

  int idle = kTcpKeepAliveSecs;
  int interval = kTcpKeepAliveProbeInterval;
  int nbprobes = kTcpKeepAliveProbes;

  if (!SocketUtil::SetKeepIdleParams(fd, idle, nbprobes, interval)) {
    return false;
  }

  if (SocketUtil::SetTCPNoDelay(fd)) {
    return false;
  }

  return true;
}

} // namespace net
