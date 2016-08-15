#ifndef NET_IP_UTIL_H_
#define NET_IP_UTIL_H_
#include <set>
#include <string>

struct sockaddr_in;

namespace net {

using IPAddressSet = std::set<std::string>;

class IpUtil {
 public:
  static bool GetIPAddressHost(IPAddressSet& iset);
  static std::string GetHostName();
  static bool GetIPAddress(IPAddressSet& iset);
  static bool GetIPV6Address(IPAddressSet& iset);
  static bool CheckIPAddress(const std::string& ip_address, const IPAddressSet& iset);
  static bool GetAddressInfo(struct sockaddr_in& ret, const char* host, int family, int type);

};

} // namespace net
#endif // NET_IP_UTIL_H_
