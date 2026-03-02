// Copyright (c) 2018 8th Wall, Inc.
// Original Author: Nicholas Butko (nb@8thwall.com)

#include "bzl/inliner/rules.h"

cc_binary {
  deps = {
    "//bzl/inliner:rules",
    "//c8:c8-log",
    "//c8/protolog:remote-service-discovery",
  };
}

#include <iostream>
#include "c8/c8-log.h"
#include "c8/protolog/remote-service-discovery.h"

using namespace c8;

void printServerListPrompt(RemoteServiceDiscovery &rsd) {
  auto servers = rsd.getServerList();
  if (servers.empty()) {
    C8Log("%s", "No servers; wait for service discovery or enter -1 to quit:");
    return;
  }

  C8Log("Discovered %d servers:", servers.size());
  for (int i = 0; i < servers.size(); ++i) {
    C8Log("%3d: %s", i, servers[i].c_str());
  }
  C8Log("%s", "Enter a server index to see connection info, or -1 to quit:");
}

int main(int argc, char *argv[]) {
  RemoteServiceDiscovery rsd;

  rsd.setServerListUpdatedCallback([](RemoteServiceDiscovery *rsd) {
    C8Log("[remote-service-discovery-main] %s", "Server list updated.");
    printServerListPrompt(*rsd);
  });

  C8Log("[remote-service-discovery-main] %s", "Starting browsing.");
  rsd.startBrowsing();
  C8Log("[remote-service-discovery-main] %s", "Browsing started.");

  printServerListPrompt(rsd);
  bool done = false;
  while (!done) {
    int choice;
    std::cin >> choice;
    if (choice < 0) {
      done = true;
    }

    auto servers = rsd.getServerList();
    if (choice >= servers.size()) {
      C8Log("%s", "Invalid choice.");
      printServerListPrompt(rsd);
      continue;
    }

    rsd.fetchServiceInfo(servers[choice], [](RemoteServiceDiscovery::ServiceInfo info, RemoteServiceDiscovery *rsd) {
      C8Log("%s", "fetchServiceInfo:");
      C8Log("  interfaceIndex: %d", info.interfaceIndex);
      C8Log("  serviceName:    %s", info.serviceName.c_str());
      C8Log("  regtype:        %s", info.regtype.c_str());
      C8Log("  replyDomain:    %s", info.replyDomain.c_str());
      C8Log("  fullName:       %s", info.fullName.c_str());
      C8Log("  hostTarget:     %s", info.hostTarget.c_str());
      C8Log("  port:           %d", info.port);
      C8Log("  hostname:       %s", info.hostname.c_str());
      C8Log("  address:        %s", info.address.c_str());
      printServerListPrompt(*rsd);
    });

    C8Log("Connection info for %s:", servers[choice].c_str());
    C8Log("%s", "TODO(nb): implement me.");
    printServerListPrompt(rsd);
  }

  return 0;
}
