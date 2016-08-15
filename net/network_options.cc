#include "net/network_options.h"
#include <arpa/inet.h>

namespace net {

namespace {

static const std::string kDefaultHost = "localhost";
static const int kDefaultPort = 8080;
static const int kDefaultMaxPacketSize = 1024;
static const int kDefaultSocketFamily = AF_INET;

} // namespace

NetworkOptions::NetworkOptions() 
    : host_(kDefaultHost),
      port_(kDefaultPort),
      max_packet_size_(kDefaultMaxPacketSize),
      socket_family_(kDefaultSocketFamily) {}

NetworkOptions::~NetworkOptions() {}


NetworkOptions::NetworkOptions(const NetworkOptions& other)
    : host_(other.host_),
      port_(other.port_),
      max_packet_size_(other.max_packet_size_),
      socket_family_(other.socket_family_),
      sin_path_(other.sin_path_) {}

void NetworkOptions::set_host(const std::string& host) { host_ = host; }
std::string NetworkOptions::host() const { return host_; }

void NetworkOptions::set_port(int port) { port_ = port; }
int NetworkOptions::port() const { return port_; }

void NetworkOptions::set_max_packet_size(uint32_t max_packet_size) { 
  max_packet_size_ = max_packet_size;                                                                     
}

uint32_t NetworkOptions::max_packet_size() const { return max_packet_size_; }

void NetworkOptions::set_socket_family(int family) { socket_family_ = family; }
int NetworkOptions::socket_family() const { return socket_family_; }

void NetworkOptions::set_sin_path(const std::string& sin_path) { sin_path_ = sin_path; }
std::string NetworkOptions::sin_path() const { return sin_path_; }

} // namespace net
