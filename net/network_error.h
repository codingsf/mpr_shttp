#ifndef NET_NETWORK_ERROR_H_
#define NET_NETWORK_ERROR_H_

namespace net {

enum NetworkErrorCode {
  OK = 0,
  CONNECT_ERROR,
  READ_ERROR,
  WRITE_ERROR,
  CLOSE_ERROR,
  INVALID_CONNECTION,
  DUPLICATE_START,
  INVALID_PACKET,
  DUPLICATE_PACKET_ID,
  TIMEOUT
};

} // namespace net
#endif // NET_NETWORK_ERROR_H_
