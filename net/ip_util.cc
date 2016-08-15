#include "net/ip_util.h"

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <ifaddrs.h>
#include <netdb.h>

#include <stdio.h>
#include <unistd.h>
#include <string>

#include <glog/logging.h>

namespace net {

bool IpUtil::CheckIPAddress(const std::string& ip_address, const IPAddressSet& iset) {
  auto it = iset.find(ip_address);
  return it != iset.end() ? true : false;
}

std::string IpUtil::GetHostName() {
  char hostname[BUFSIZ];
  struct addrinfo hints, *info;
  int gai_result;

  ::gethostname(hostname, sizeof(hostname));

  memset(&hints, 0, sizeof(hints));
  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_flags = AI_CANONNAME;

  if ((gai_result = ::getaddrinfo(hostname, nullptr, &hints, &info)) != 0) {
    return std::string(hostname);
  }

  if (!info) {
    return std::string(hostname);
  }

  std::string name = info->ai_canonname;
  ::freeaddrinfo(info);
  return name;
}

bool IpUtil::GetIPAddressHost(IPAddressSet& iset) {
  if (!GetIPAddress(iset)) {
    return false;
  }
  iset.insert(GetHostName());
  return true;
}

bool IpUtil::GetIPAddress(IPAddressSet& iset) {
  struct ifaddrs* if_addrs = nullptr;
  struct ifaddrs* ifa = nullptr;

  iset.clear();
  ::getifaddrs(&if_addrs);

  for (ifa = if_addrs; ifa != nullptr; ifa = ifa->ifa_next) {
    if (ifa->ifa_addr->sa_family == AF_INET) {
      struct sockaddr_in* addr_in = (struct sockaddr_in *)ifa->ifa_addr;
      char addr_buf[INET_ADDRSTRLEN];
      inet_ntop(AF_INET, &(addr_in->sin_addr), addr_buf, INET_ADDRSTRLEN);
      if (strcmp("lo", ifa->ifa_name) != 0) {
        iset.insert(std::string(addr_buf));
      }
    }
  }
  if (if_addrs != nullptr) {
    ::freeifaddrs(if_addrs);
  }
  return iset.empty() ? false : true;
}

bool IpUtil::GetAddressInfo(struct sockaddr_in& t, const char* host, int family, int type) {
  struct addrinfo hints, *res;

  memset(&hints, 0, sizeof(hints));
  hints.ai_family = family;
  hints.ai_socktype = type;
  int err = ::getaddrinfo(host, nullptr, &hints, &res);
  if (err == 0) {
    t = *(struct sockaddr_in *) res->ai_addr;
    ::freeaddrinfo(res);
    return true;
  }
  return false;
}

} // namespace net
