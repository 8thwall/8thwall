// Copyright (c) 2017 8th Wall, Inc.
// Original Author: Erik Murphy-Chutorian (mc@8thwall.com)
//
// C-wrapper functions for client code to communicate with the RemoteService server.

#pragma once

#include <event2/event.h>
#include <list>
#include <memory>
#include <mutex>
#include <queue>
#include <string>
#include <thread>
#include "c8/map.h"
#include "c8/network/dns-service-discovery.h"
#include "c8/string.h"
#include "c8/vector.h"

namespace c8 {

class RemoteServiceDiscovery {
public:
  struct ServiceInfo {
    uint32_t interfaceIndex = 0;  // Browse
    String displayName = "";      // Browse
    String serviceName = "";      // Browse
    String regtype = "";          // Browse
    String replyDomain = "";      // Browse
    String fullName = "";         // Resolve
    String hostTarget = "";       // Resolve
    uint16_t port = 0;            // Resolve
    String hostname = "";         // DNS
    String address = "";          // DNS
  };

  RemoteServiceDiscovery();
  ~RemoteServiceDiscovery();

  // Retrieve list of servers
  Vector<String> getServerList();

  void startBrowsing();

  using ServerListUpdatedCallback = std::function<void(RemoteServiceDiscovery *)>;
  void setServerListUpdatedCallback(ServerListUpdatedCallback callback);

  using ServiceInfoCallback = std::function<void(ServiceInfo, RemoteServiceDiscovery *)>;
  void fetchServiceInfo(const String &serviceName, ServiceInfoCallback callback);
  void setWifiInterfaceIndex(int index) { wifiInterfaceIndex_ = index; }

private:
  evutil_socket_t writeFd_ = 0;

  std::thread thrd_;
  Vector<String> serverList_;
  Vector<String> addingServerList_;
  TreeMap<String, DnsServiceAd> browseInfo_;
  Vector<std::thread> fetchThreads_;
  int wifiInterfaceIndex_ = -1;

  std::mutex serverListLock_;
  std::unique_ptr<ServerListUpdatedCallback> serverListUpdatedCallback_ = nullptr;
};

}  // namespace c8
