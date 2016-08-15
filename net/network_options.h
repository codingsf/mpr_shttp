#ifndef NET_NETWORK_OPTIONS_H_
#define NET_NETWORK_OPTIONS_H_
#include <string>
#include "base/macros.h"

namespace net {

class NetworkOptions {
 public:
  NetworkOptions();
  NetworkOptions(const NetworkOptions& other);
  ~NetworkOptions();

  void set_host(const std::string& host);
  void set_port(int port);
  std::string host() const;
  int port() const;

  void set_max_packet_size(uint32_t new_value);
  uint32_t max_packet_size() const;

  void set_socket_family(int family);
  int socket_family() const;

  void set_sin_path(const std::string& sin_path);
  std::string sin_path() const;

 private:
  std::string host_;
  int port_;
  uint32_t max_packet_size_;
  int socket_family_;
  std::string sin_path_;    // For UnixDomain

};

} // namespace net
#endif // NET_NETWORK_OPTIONS_H_
