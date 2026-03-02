#include "bzl/inliner/rules.h"

cc_library {
  hdrs = {"remote-service-discovery.h"};
  deps = {
    "//bzl/inliner:rules",
    "//c8:c8-log",
    "//c8:exceptions",
    "//c8:map",
    "//c8:string",
    "//c8:vector",
    "//c8/events:kj-event-listener",
    "//c8/events:lev-event-listener",
    "//c8/network:dns-service-discovery",
    "@capnproto//:kj",
  };
  visibility = {":protolog-pkgs"};
}

#include "c8/protolog/remote-service-discovery.h"

#include <kj/async-io.h>
#include <kj/async-unix.h>
#include <kj/async.h>
#include <kj/common.h>
#include <kj/debug.h>

#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <algorithm>
#include <ctime>
#include <thread>

#include "c8/c8-log.h"
#include "c8/events/kj-event-listener.h"
#include "c8/events/lev-event-listener.h"
#include "c8/exceptions.h"
#include "c8/network/dns-service-discovery.h"
#include "c8/string.h"
#include "c8/vector.h"

namespace c8 {
namespace {

kj::Promise<DnsServiceAddress> dnsLookup(FdEventListener &fdListener, DnsServiceInfo info);

// TODO(nb): Add "8th wall" to this name, and share value with streaming server.
static constexpr char SERVICE_NAME[] = "_8thWall_remote_service._tcp";
static constexpr char terminateChar = 't';

void listenForTerminateSignal(evutil_socket_t readFd, std::function<void()> terminate);

}  // namespace

RemoteServiceDiscovery::RemoteServiceDiscovery() {}

void RemoteServiceDiscovery::setServerListUpdatedCallback(ServerListUpdatedCallback callback) {
  std::unique_ptr<decltype(callback)> heapCallback(new decltype(callback)(std::move(callback)));
  serverListUpdatedCallback_ = std::move(heapCallback);
}

void RemoteServiceDiscovery::startBrowsing() {
  C8Log("[remote-service-discovery] %s", "startBrowsing");

  // TODO(nb): Switch to libevent.
  int pipefds[2];
  KJ_SYSCALL(pipe(pipefds));
  evutil_socket_t readFd = pipefds[0];
  writeFd_ = pipefds[1];

  C8Log("[remote-service-discovery] %s", "start browse thread");
  thrd_ = std::thread([this, readFd]() {
    C8Log("[remote-service-discovery] %s", "run browse thread");
    // TODO(nb): Switch to libevent.
    KjEventListener fdListener;

    kj::Vector<kj::Promise<void>> tasks;
    std::unique_ptr<DnsServiceDiscovery> browseDns;

    auto terminate = [&browseDns, &fdListener, &tasks, readFd]() {
      C8Log("[remote-service-discovery] %s", "terminate");
      // Clean up the browseDns callback.
      browseDns.reset();

      // Remove the inter-thread event listener and stop listening for events, but defer since this
      // will be called from within the read callback;
      auto defer = kj::evalLater([&fdListener, readFd]() {
                     C8Log("[remote-service-discovery] %s", "stop listener");
                     fdListener.removeFdEvent(readFd);
                     fdListener.stop();
                   })
                     .eagerlyEvaluate(nullptr);
      tasks.add(std::move(defer));
    };

    fdListener.addFdEvent(
      readFd,
      EventFlag::READ | EventFlag::EDGE_TRIGGER | EventFlag::PERSIST,
      [readFd, terminate]() { listenForTerminateSignal(readFd, terminate); });

    // Browse for RemoteService Services
    C8Log("[remote-service-discovery] %s", "add browse query");
    auto browseQuery = DnsServiceDiscovery::newBrowseRequest(
      SERVICE_NAME,
      fdListener,
      [this](DnsServiceAd serviceAd, bool add, bool more) {
        {
          if (wifiInterfaceIndex_ > 0 && wifiInterfaceIndex_ != serviceAd.interfaceIndex) {
            serviceAd.displayName = "USB";
          } else {
            serviceAd.displayName = serviceAd.serviceName;
          }
          C8Log("[remote-service-discovery] browse callback: %s; add: %d; more: %d", serviceAd.displayName.c_str(), add, more);
          std::lock_guard<std::mutex> lock(serverListLock_);
          auto addListIt = std::find(
            this->addingServerList_.begin(), this->addingServerList_.end(), serviceAd.displayName);
          auto serverListIt =
            std::find(this->serverList_.begin(), this->serverList_.end(), serviceAd.displayName);

          bool foundInAddList = addListIt != this->addingServerList_.end();
          bool foundInServerList = serverListIt != this->serverList_.end();
          if (!add) {
            if (foundInAddList) {
              this->addingServerList_.erase(addListIt);
            }
            if (foundInServerList) {
              this->serverList_.erase(serverListIt);
            }
          } else {
            if (!foundInServerList && !foundInAddList) {
              C8Log(
                "[remote-service-discovery] adding server to list \"%s\"",
                serviceAd.displayName.c_str());
              this->browseInfo_[serviceAd.displayName] = serviceAd;
              this->addingServerList_.push_back(serviceAd.displayName);
            }
          }

          // Wait to collect all services
          if (more) {
            return;
          }
          C8Log("[remote-service-discovery] %s", "commit servers to exported list");
          this->serverList_.insert(
            this->serverList_.end(),
            this->addingServerList_.begin(),
            this->addingServerList_.end());
          this->addingServerList_.clear();
        }

        if (serverListUpdatedCallback_ != nullptr) {
          C8Log("[remote-service-discovery] %s", "notifying callback");
          (*serverListUpdatedCallback_)(this);
          C8Log("[remote-service-discovery] %s", "end browse query");
        }
      });  // end browseQuery
    browseDns.reset(new DnsServiceDiscovery(std::move(browseQuery)));

    C8Log("[remote-service-discovery] %s", "browse thread waiting for terminate");
    fdListener.wait();
    close(readFd);
    C8Log("[remote-service-discovery] %s", "browse thread ready for join");
  });  // end std::thread
  C8Log("[remote-service-discovery] %s", "startBrowsing done");
}

Vector<String> RemoteServiceDiscovery::getServerList() {
  Vector<String> serverListCopy;
  {
    std::lock_guard<std::mutex> lock(serverListLock_);
    serverListCopy = serverList_;
  }
  return serverListCopy;
}

RemoteServiceDiscovery::~RemoteServiceDiscovery() {
  C8Log("[remote-service-discovery] %s", "~RemoteServiceDiscovery");
  // Detach thread for simplicity, but alternatively could add support for
  // a clean termination and join.
  KJ_SYSCALL(write(writeFd_, &terminateChar, sizeof(terminateChar)));
  if (thrd_.joinable()) {
    thrd_.join();
  }
  for (auto &thrd : fetchThreads_) {
    if (thrd.joinable()) {
      thrd.join();
    }
  }
  close(writeFd_);  // don't close before flush
}

void RemoteServiceDiscovery::fetchServiceInfo(
  const String &serviceName, ServiceInfoCallback callback) {

  // TODO(nb): Switch to libevent.
  int pipefds[2];
  KJ_SYSCALL(pipe(pipefds));
  evutil_socket_t readFd = pipefds[0];
  evutil_socket_t writeFd = pipefds[1];

  C8Log("[remote-service-discovery] %s \"%s\"", "resolve-thread start", serviceName.c_str());
  fetchThreads_.push_back(std::thread([this, readFd, writeFd, serviceName, callback]() {
    // TODO(nb): Switch to libevent.
    KjEventListener fdListener;

    kj::Vector<kj::Promise<void>> tasks;

    bool foundInServerList = false;
    {
      std::lock_guard<std::mutex> lock(serverListLock_);
      auto serverListIt =
        std::find(this->serverList_.begin(), this->serverList_.end(), serviceName);
      foundInServerList = serverListIt != this->serverList_.end();
    }

    if (!foundInServerList) {
      callback(ServiceInfo(), this);
      return;
    }

    // Create a new resolve query.
    auto serverAd = this->browseInfo_[serviceName];
    C8Log("[remote-service-discovery] %s \"%s\"", "resolve-thread start resolve lookup", serverAd.displayName.c_str());
    auto request = DnsServiceDiscovery::newResolveRequest(
      serverAd,
      fdListener,
      [this, &fdListener, &tasks, serverAd, readFd, callback](DnsServiceInfo info, bool more) {
        C8Log("[remote-service-discovery] %s", "resolve-thread resolve done, start dns lookup");
        auto connection = dnsLookup(fdListener, info)
                            .then([this, &fdListener, info, serverAd, readFd, callback](
                                    DnsServiceAddress serviceAddress) {
                              ServiceInfo fullInfo;
                              fullInfo.interfaceIndex = serverAd.interfaceIndex;
                              fullInfo.displayName = serverAd.displayName;
                              fullInfo.serviceName = serverAd.serviceName;
                              fullInfo.regtype = serverAd.regtype;
                              fullInfo.replyDomain = serverAd.replyDomain;
                              fullInfo.fullName = info.fullName;
                              fullInfo.hostTarget = info.hostTarget;
                              fullInfo.port = info.port;
                              fullInfo.hostname = serviceAddress.hostname;
                              fullInfo.address = serviceAddress.address;
                              callback(fullInfo, this);
                              C8Log("[remote-service-discovery] %s", "dns lookup done");
                              fdListener.removeFdEvent(readFd);
                              fdListener.stop();
                            })
                            .eagerlyEvaluate(nullptr);
        tasks.add(std::move(connection));
      });

    C8Log("[remote-service-discovery] %s", "resolve-thread wait for dns lookup");
    fdListener.wait();
    C8Log("[remote-service-discovery] %s", "resolve-thread done, close pipes and ready for join.");
    close(readFd);
    close(writeFd);
    // TODO(nb): Remove this thread from fetchThreads_ when done.
  }));  // end std::thread
}

namespace {

// Look up the IPv4 or IPv6 address for the provided DNS address.
kj::Promise<DnsServiceAddress> dnsLookup(FdEventListener &fdListener, DnsServiceInfo info) {

  // Create a new promise to satisfy with the DNS lookup
  auto pair = kj::newPromiseAndFulfiller<DnsServiceAddress>();

  DnsServiceDiscovery getAddrInfoDns = DnsServiceDiscovery::newGetAddrInfoRequest(
    info,
    fdListener,
    [fulfiller = pair.fulfiller.get()](DnsServiceAddress serviceAddress, bool more) {
      fulfiller->fulfill(std::move(serviceAddress));
    });
  auto promise = pair.promise.attach(kj::mv(pair.fulfiller), std::move(getAddrInfoDns));
  return kj::mv(promise);
}

// Drain any bytes on the read File descriptor, then send the current queue to each of the connected
//  Rpc servers.
void listenForTerminateSignal(evutil_socket_t readFd, std::function<void()> terminate) {
  C8Log("[remote-service-discovery] %s", "Scanning fd for terminate signal");
  char readBuffer[4096];
  ssize_t n = 0;
  do {
    KJ_SYSCALL(n = read(readFd, &readBuffer, sizeof(readBuffer)));
    char *endByte = readBuffer + n;
    if (std::find(readBuffer, endByte, terminateChar) != endByte) {
      C8Log("[remote-service-discovery] %s", "Found terminate signal on fd; terminating");
      // End all processing tasks if the terminate char is encountered.
      terminate();
      return;
    }
  } while (n == sizeof(readBuffer));
}

}  // namespace

}  // namespace c8
