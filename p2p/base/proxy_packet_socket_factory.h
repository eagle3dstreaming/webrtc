#ifndef P2P_BASE_PROXY_PACKET_SOCKET_FACTORY_H_
#define P2P_BASE_PROXY_PACKET_SOCKET_FACTORY_H_

#include <string>

#include "p2p/base/basic_packet_socket_factory.h"

namespace rtc {

class SocketFactory;
class Thread;

class ProxyPacketSocketFactory : public BasicPacketSocketFactory {
 public:
  ProxyPacketSocketFactory();
  explicit ProxyPacketSocketFactory(Thread* thread);
  explicit ProxyPacketSocketFactory(SocketFactory* socket_factory);
  ~ProxyPacketSocketFactory() override;

  void SetProxyInformation(const ProxyInfo& proxy_info);

  AsyncPacketSocket* CreateClientTcpSocket(
      const SocketAddress& local_address,
      const SocketAddress& remote_address,
      const ProxyInfo& proxy_info,
      const std::string& user_agent,
      const PacketSocketTcpOptions& tcp_options) override;

 private:
  struct ProxyInfo proxy_info_;
};

}  // namespace rtc

#endif  // P2P_BASE_PROXY_PACKET_SOCKET_FACTORY_H_
