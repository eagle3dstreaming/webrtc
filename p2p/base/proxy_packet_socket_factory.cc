#include "p2p/base/proxy_packet_socket_factory.h"

namespace rtc {

ProxyPacketSocketFactory::ProxyPacketSocketFactory()
    : BasicPacketSocketFactory() {}

ProxyPacketSocketFactory::ProxyPacketSocketFactory(Thread* thread)
    : BasicPacketSocketFactory(thread) {}

ProxyPacketSocketFactory::ProxyPacketSocketFactory(
    SocketFactory* socket_factory)
    : BasicPacketSocketFactory(socket_factory) {}

ProxyPacketSocketFactory::~ProxyPacketSocketFactory() {}

void ProxyPacketSocketFactory::SetProxyInformation(const ProxyInfo& proxy_info) {
  proxy_info_ = proxy_info;
}

AsyncPacketSocket* ProxyPacketSocketFactory::CreateClientTcpSocket(
    const SocketAddress& local_address,
    const SocketAddress& remote_address,
    const ProxyInfo& proxy_info,
    const std::string& user_agent,
    const PacketSocketTcpOptions& tcp_options) {
  return BasicPacketSocketFactory::CreateClientTcpSocket(local_address, 
                                                         remote_address,
                                                         proxy_info_,
                                                         user_agent,
                                                         tcp_options);
}

}
