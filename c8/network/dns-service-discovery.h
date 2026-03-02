// Copyright (c) 2017 8th Wall, Inc.
// Original Author: Erik Murphy-Chutorian (mc@8thwall.com)

#pragma once

#include <dns_sd.h>
#include <memory>
#include <string>
#include <type_traits>

#include "c8/events/event-listener.h"

namespace c8 {

// Information about an mDNS service advertised on a network.
struct DnsServiceAd {
  uint32_t interfaceIndex;
  std::string displayName;
  std::string serviceName;
  std::string regtype;
  std::string replyDomain;
};

// Information about an mDNS service resolved on a network. Unlike dns-sd.h, the
// port here is in host byte-order.
struct DnsServiceInfo {
  uint32_t interfaceIndex;
  std::string fullName;
  std::string hostTarget;
  uint16_t port;
};

// IP Address information for a resolved service.
struct DnsServiceAddress {
  uint32_t interfaceIndex;
  std::string hostname;
  std::string address;
};

// DnsServiceDiscovery is modern C++ wrapper around the dns-sd library for DNS
// service discovery. It provides exception-safety and a simplified interace
// that supports std::function callbacks for use with lambda captures.
// Connections to the mDNS daemon are asynchronous and require an
// FdEventListener implementation.
class DnsServiceDiscovery {
 public:
  // Create a new mDNS connection for a registration request. The registered
  // service will be unregistered when the returned object goes out of scope.
  // Throws a runtime error on failure.
  //   regtype - Service mDNS name, e.g. _myservice._tcp
  //   port - Service port, in host (normal) byte-order.
  //   fdListener - An FdEventListener to run the mDNS callbacks.
  using RegisterCallback = std::function<void()>;
  static DnsServiceDiscovery newRegisterRequest(
    const char* regtype,
    uint16_t port,
    FdEventListener& fdListener,
    RegisterCallback callback);

  // Create a new mDNS connection for a browse request. If 'add' is true, the
  // service is newly added and if false, the service is newly removed. If
  // 'more' is true, the callback will be immediately called with subsequent
  // information. Throws a runtime error on failure.
  using BrowseCallback = std::function<void(DnsServiceAd, bool add, bool more)>;
  static DnsServiceDiscovery newBrowseRequest(
    const char* regtype, FdEventListener& fdListener, BrowseCallback callback);

  // Create a new mDNS connection for a resolve request. The callback provides
  // the resolved service information along with a 'more' bool returning true if
  // the callback will be immediately called with subsequent information.
  // Throws a runtime error on failure.
  using ResolveCallback = std::function<void(DnsServiceInfo, bool more)>;
  static DnsServiceDiscovery newResolveRequest(
    const DnsServiceAd& serviceAd,
    FdEventListener& fdListener,
    ResolveCallback callback);

  // Create a new mDNS connection for a get address request. The callback provides
  // the IPv4 or IPv6 address along with a 'more' bool returning true if
  // the callback will be immediately called with subsequent information.
  // Throws a runtime error on failure.
  using GetAddrInfoCallback = std::function<void(DnsServiceAddress, bool more)>;
  static DnsServiceDiscovery newGetAddrInfoRequest(
    const DnsServiceInfo& serviceInfo,
    FdEventListener& fdListener,
    GetAddrInfoCallback callback);

  // Destructor.
  ~DnsServiceDiscovery() = default;

  // Move constructor.
  DnsServiceDiscovery(DnsServiceDiscovery&& rhs) = default;

  // Disallow copy and assign.
  DnsServiceDiscovery(const DnsServiceDiscovery&) = delete;
  DnsServiceDiscovery& operator=(const DnsServiceDiscovery&) = delete;

 private:
  // Private constructor. To create a DnsServiceDiscovery, use one of the static
  // new* methods to create the desired connection type.
  DnsServiceDiscovery(
    DNSServiceRef ref, int trackedFd, FdEventListener* listener) noexcept
      : serviceRef(ref, &DNSServiceRefDeallocate), fd(trackedFd, listener) {}

  std::unique_ptr<
    std::remove_pointer_t<DNSServiceRef>,
    decltype(&DNSServiceRefDeallocate)>
    serviceRef;

  // Scope guard for a file descriptor that needs to be unregistered.
  class TrackedFd {
   public:
    TrackedFd(int fd_, FdEventListener* listener)
        : fd(fd_), fdListener(listener) {}
    ~TrackedFd() {
      if (fd) {
        fdListener->removeFdEvent(fd);
      }
    }
    TrackedFd(TrackedFd&& rhs) {
      fd = rhs.fd;
      fdListener = rhs.fdListener;
      rhs.fd = 0;
    }

    TrackedFd(const TrackedFd&) = delete;
    TrackedFd& operator=(const TrackedFd&) = delete;

   private:
    int fd;
    FdEventListener* fdListener;
  };

  TrackedFd fd;

  // A DnsServiceDiscovery will have only one of the following callbacks.
  std::unique_ptr<RegisterCallback> registerCallback;
  std::unique_ptr<BrowseCallback> browseCallback;
  std::unique_ptr<ResolveCallback> resolveCallback;
  std::unique_ptr<GetAddrInfoCallback> getAddrInfoCallback;
};

}  // namespace c8
