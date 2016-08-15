#ifndef NET_SOCKET_UTIL_H_
#define NET_SOCKET_UTIL_H_
#include "base/macros.h"

namespace net {

class SocketUtil {
 public:
  static bool SetNonBlocking(int fd);

  static bool GetSendBufferSize(int fd, int& ret);
  static bool SetSendBufferSize(int fd, int size);

  static bool GetRecvBufferSize(int fd, int& ret);
  static bool SetRecvBufferSize(int fd, int size);

  static bool SetReuseAddress(int fd);

  static bool SetKeepAlive(int fd);
  static bool SetKeepIdleTime(int fd, int time);
  static bool SetKeepIdleCount(int fd, int count);
  static bool SetKeepIdleInterval(int fd, int interval);
  static bool SetKeepIdleParams(int fd, int time, int count, int interval);
  static bool SetTCPNoDelay(int fd);

  static bool SetSocketDefaults(int fd);
};

} // namespace net
#endif // NET_SOCKET_UTIL_H_
