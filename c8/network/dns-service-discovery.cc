#include "c8/network/dns-service-discovery.h"

#ifdef WIN32
#include <Ws2tcpip.h>
#else
#include <arpa/inet.h>
#endif

#include <string>
#include "c8/c8-log.h"
#include "c8/exceptions.h"

using std::string;

namespace c8 {

namespace {

void registerReply(
  DNSServiceRef sdRef,
  DNSServiceFlags flags,
  DNSServiceErrorType errorCode,
  const char* name,
  const char* regtype,
  const char* domain,
  void* context) {
  if (errorCode != kDNSServiceErr_NoError) {
    C8Log("[dns-service-discovery] Register Reply Cb Error %s", std::to_string(errorCode).c_str());
    C8_THROW("DNS Service Error code=" + std::to_string(errorCode));
  }

  auto& callback =
    *static_cast<DnsServiceDiscovery::RegisterCallback*>(context);
  callback();
}

void browseReply(
  DNSServiceRef sdRef,
  DNSServiceFlags flags,
  uint32_t interfaceIndex,
  DNSServiceErrorType errorCode,
  const char* serviceName,
  const char* regtype,
  const char* replyDomain,
  void* context) {

  if (errorCode != kDNSServiceErr_NoError) {
    C8_THROW(
      "DNS Service Discovery Error code=" + std::to_string(errorCode));
  }

  DnsServiceAd ad;
  ad.interfaceIndex = interfaceIndex;
  ad.serviceName = serviceName;
  ad.regtype = regtype;
  ad.replyDomain = replyDomain;

  auto& callback = *static_cast<DnsServiceDiscovery::BrowseCallback*>(context);
  bool addService = flags & kDNSServiceFlagsAdd;
  bool moreComing = flags & kDNSServiceFlagsMoreComing;
  callback(ad, addService, moreComing);
}

void resolveReply(
  DNSServiceRef sdRef,
  DNSServiceFlags flags,
  uint32_t interfaceIndex,
  DNSServiceErrorType errorCode,
  const char* fullName,
  const char* hostTarget,
  uint16_t port,
  uint16_t txtLen,
  const unsigned char* txtRecord,
  void* context) {

  if (errorCode != kDNSServiceErr_NoError) {
    C8_THROW(
      "DNS Service Discovery Error code=" + std::to_string(errorCode));
  }

  DnsServiceInfo info;
  info.interfaceIndex = interfaceIndex;
  info.fullName = fullName;
  info.hostTarget = hostTarget;
  info.port = ntohs(port);

  auto& callback = *static_cast<DnsServiceDiscovery::ResolveCallback*>(context);
  bool moreResults = flags & kDNSServiceFlagsMoreComing;
  callback(info, moreResults);
}

void getAddrInfoReply(
  DNSServiceRef sdRef,
  DNSServiceFlags flags,
  uint32_t interfaceIndex,
  DNSServiceErrorType errorCode,
  const char* hostname,
  const struct sockaddr* address,
  uint32_t ttl,
  void* context) {

  if (errorCode != kDNSServiceErr_NoError) {
    C8_THROW(
      "DNS Service Discovery Error code=" + std::to_string(errorCode));
  }

  DnsServiceAddress serviceAddress;
  serviceAddress.interfaceIndex = interfaceIndex;
  serviceAddress.hostname = hostname;

  switch (address->sa_family) {
    case AF_INET: {
      // This is an IPv4 address.
      auto ipv4 = reinterpret_cast<const struct sockaddr_in*>(address);
      serviceAddress.address.resize(INET_ADDRSTRLEN);
      inet_ntop(
        AF_INET,
        &(ipv4->sin_addr),
        &serviceAddress.address[0],
        INET_ADDRSTRLEN);
      break;
    }
    case AF_INET6: {
      // This is an IPv6 address.
      auto ipv6 = reinterpret_cast<const struct sockaddr_in6*>(address);
      serviceAddress.address.resize(INET6_ADDRSTRLEN);
      inet_ntop(
        AF_INET6,
        &(ipv6->sin6_addr),
        &serviceAddress.address[0],
        INET6_ADDRSTRLEN);
      break;
    }
    default: {
      C8_THROW(
        "DNS Service Discovery unexpected family " +
        std::to_string(address->sa_family));
    }
  }

  auto& callback =
    *static_cast<DnsServiceDiscovery::GetAddrInfoCallback*>(context);
  bool moreResults = flags & kDNSServiceFlagsMoreComing;
  callback(serviceAddress, moreResults);
}

}  // namespace

DnsServiceDiscovery DnsServiceDiscovery::newRegisterRequest(
  const char* regtype,
  uint16_t port,
  FdEventListener& fdListener,
  RegisterCallback callback) {
  // Move the callback onto the heap.
  std::unique_ptr<decltype(callback)> heapCallback(
    new decltype(callback)(std::move(callback)));

  C8Log("[dns-service-discovery] %s", "Initiate the DNS service browse request");
  // Initiate the DNS service browse request.
  DNSServiceRef ref;
  auto error = DNSServiceRegister(
    &ref,            // DNSServiceRef * sdRef
    0,               // DNSServiceFlags flags
    0,               // uint32_t interfaceIndex
    nullptr,         // const char* name - nullptr uses computer name
    regtype,         // const char* regtype
    nullptr,         // const char* domain - nullptr uses default
    nullptr,         // const char* host - nullptr uses default
    htons(port),     // uint16_t port in network byte order
    0,               // uint16_t txtLen - the length of the txtRecord in bytes
    nullptr,         // const void *txtRecord - nullptr means not TXT record
    &registerReply,  // DNSServiceRegisterReply callBack
    heapCallback.get()  // void *context
    );

  // Add a file descriptor event to call the service process callback when the
  // browse query is complete.
  auto fd = DNSServiceRefSockFD(ref);
  fdListener.addFdEvent(
    fd,
    EventFlag::READ | EventFlag::EDGE_TRIGGER | EventFlag::PERSIST,
    [ref]() { DNSServiceProcessResult(ref); });

  // Create and return the service discovery object.
  DnsServiceDiscovery dns(ref, fd, &fdListener);

  if (error == kDNSServiceErr_ServiceNotRunning) {
    C8Log("[dns-service-discovery] %s", "DNS Service Not Running");
    // We permit USB connections so execute the callback
    auto &cb = *static_cast<DnsServiceDiscovery::RegisterCallback *>(heapCallback.get());
    cb();
    return dns;
  }

  if (error != kDNSServiceErr_NoError) {
    C8Log("[dns-service-discovery] New Register Req Error %s", std::to_string(error).c_str());
    C8_THROW(
      "DNS Service Discovery Error code=" + std::to_string(error));
  }

  dns.registerCallback = std::move(heapCallback);
  return dns;
}

DnsServiceDiscovery DnsServiceDiscovery::newBrowseRequest(
  const char* regtype, FdEventListener& fdListener, BrowseCallback callback) {
  // Move the callback onto the heap.
  std::unique_ptr<decltype(callback)> heapCallback(
    new decltype(callback)(std::move(callback)));

  // Initiate the DNS service browse request.
  DNSServiceRef ref;
  auto error = DNSServiceBrowse(
    &ref,               // DNSServiceRef * sdRef
    0,                  // DNSServiceFlags flags - currently ignored
    0,                  // uint32_t interfaceIndex - browse all interfaces
    regtype,            // const char* regtype
    nullptr,            // const char* domain
    browseReply,        // DNSServiceBrowseReply callBack
    heapCallback.get()  // callback context
    );

  if (error != kDNSServiceErr_NoError) {
    C8_THROW(
      "DNS Service Discovery Error code=" + std::to_string(error));
  }

  // Add a file descriptor event to call the service process callback when the
  // browse query is complete.
  auto fd = DNSServiceRefSockFD(ref);
  fdListener.addFdEvent(
    fd,
    EventFlag::READ | EventFlag::EDGE_TRIGGER | EventFlag::PERSIST,
    [ref]() { DNSServiceProcessResult(ref); });

  // Create and return the service discovery object.
  DnsServiceDiscovery dns(ref, fd, &fdListener);
  dns.browseCallback = std::move(heapCallback);
  return dns;
}

DnsServiceDiscovery DnsServiceDiscovery::newResolveRequest(
  const DnsServiceAd& serviceAd,
  FdEventListener& fdListener,
  ResolveCallback callback) {
  // Move the callback onto the heap.
  std::unique_ptr<decltype(callback)> heapCallback(
    new decltype(callback)(std::move(callback)));

  // Initiate the DNS service resolve request.
  DNSServiceRef ref;
  auto error = DNSServiceResolve(
    &ref,
    0,
    serviceAd.interfaceIndex,
    serviceAd.serviceName.c_str(),
    serviceAd.regtype.c_str(),
    serviceAd.replyDomain.c_str(),
    &resolveReply,
    heapCallback.get());

  if (error != kDNSServiceErr_NoError) {
    C8_THROW(
      "DNS Service Discovery Error code=" + std::to_string(error));
  }

  // Add a file descriptor event to call the service process callback when the
  // browse query is complete.
  auto fd = DNSServiceRefSockFD(ref);
  fdListener.addFdEvent(
    DNSServiceRefSockFD(ref),
    EventFlag::READ | EventFlag::EDGE_TRIGGER | EventFlag::PERSIST,
    [ref]() { DNSServiceProcessResult(ref); });

  // Create and return the service discovery object.
  DnsServiceDiscovery dns(ref, fd, &fdListener);
  dns.resolveCallback = std::move(heapCallback);
  return dns;
}

DnsServiceDiscovery DnsServiceDiscovery::newGetAddrInfoRequest(
  const DnsServiceInfo& serviceInfo,
  FdEventListener& fdListener,
  GetAddrInfoCallback callback) {
  // Move the callback onto the heap.
  std::unique_ptr<decltype(callback)> heapCallback(
    new decltype(callback)(std::move(callback)));

  // Initiate the DNS GetAddrInfo request.
  DNSServiceRef ref;
  auto error = DNSServiceGetAddrInfo(
    &ref,
    0,
    serviceInfo.interfaceIndex,
    kDNSServiceProtocol_IPv4,
    serviceInfo.hostTarget.c_str(),
    &getAddrInfoReply,
    heapCallback.get());

  if (error != kDNSServiceErr_NoError) {
    C8_THROW(
      "DNS Service Discovery Error code=" + std::to_string(error));
  }

  // Add a file descriptor event to call the service process callback when the
  // dns lookup.
  auto fd = DNSServiceRefSockFD(ref);
  fdListener.addFdEvent(
    DNSServiceRefSockFD(ref),
    EventFlag::READ | EventFlag::EDGE_TRIGGER,
    [ref]() { DNSServiceProcessResult(ref); });

  // Create and return the service discovery object.
  DnsServiceDiscovery dns(ref, fd, &fdListener);
  dns.getAddrInfoCallback = std::move(heapCallback);
  return dns;
}

}  // namespace c8
