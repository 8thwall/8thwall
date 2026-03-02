#include "bzl/inliner/rules.h"

cc_library {
  hdrs = {"remote-service-connection.h"};
  deps = {
    "//bzl/inliner:rules",
    "//c8:c8-log",
    "//c8:c8-log-proto",
    "//c8:exceptions",
    "//c8:string",
    "//c8:vector",
    "//c8/events:kj-event-listener",
    "//c8/events:lev-event-listener",
    "//c8/io:capnp-messages",
    "//c8/network:dns-service-discovery",
    "//c8/protolog:remote-service-discovery",
    "//c8/protolog/api:remote-service.capnp-cc",
    "@capnproto//:capnp-lib",
    "@capnproto//:kj",
  };
  visibility = {":protolog-pkgs"};
}

#include "c8/protolog/remote-service-connection.h"

#if defined(_WIN32) || defined(WIN32)
#error "This file cannot be compiled under Windows. This code is only used on mobile."
#endif

#include <capnp/rpc-twoparty.h>
#include <capnp/serialize.h>
#include <kj/async-io.h>
#include <kj/async-unix.h>
#include <kj/async.h>
#include <kj/common.h>

#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <algorithm>
#include <ctime>
#include <iostream>
#include <map>
#include <queue>
#include <set>
#include <sstream>
#include <string>
#include <thread>

#include "c8/c8-log-proto.h"
#include "c8/c8-log.h"
#include "c8/events/kj-event-listener.h"
#include "c8/events/lev-event-listener.h"
#include "c8/exceptions.h"
#include "c8/network/dns-service-discovery.h"
#include "c8/protolog/api/remote-request.capnp.h"
#include "c8/protolog/api/remote-service-interface.capnp.h"
#include "c8/string.h"
#include "c8/vector.h"
#include "reality/engine/api/base/geo-types.capnp.h"

using std::string;

namespace c8 {

namespace {

static constexpr char updateChar = 'u';
static constexpr char terminateChar = 't';

}  // namespace

RemoteServiceConnection::RemoteServiceConnection() : status_(ConnectionStatus::UNSPECIFIED) {}

/**
 * Logging is perform on a separate thread. A connection (socketpair) is open between this thread
 * and the logging thread. The logging thread will read control characters from the socket. When the
 * logging thread is active, it will wait for `update character` to start reading data from the
 * queues_.
 *
 * The main thread controls the logging thread by writing an `update character` or a `terminate
 * character`  to the socket.
 *
 * The queues_ contain a pair of queue: draining and filing. The main thread enqueues data (see
 * multilog and log methods) into the queues_.
 */
void RemoteServiceConnection::logToServer(RemoteServiceDiscovery::ServiceInfo server) {
  C8Log("[remote-service-connection] %s \"%s\"", "logToServer", server.serviceName.c_str());
  if (status_.load() != ConnectionStatus::UNSPECIFIED) {
    C8Log("[remote-service-connection] service already initialized in state %d", status_.load());
    return;
  }

  status_.store(RemoteServiceConnection::ConnectionStatus::CONNECTING);

  int pipefds[2];
  KJ_SYSCALL(pipe(pipefds));
  evutil_socket_t readFd = pipefds[0];
  writeFd_ = pipefds[1];

  if (server.address.empty() || server.port == 0) {
    C8Log(
      "[remote-service-connection] Invalid server spec: %s:%d",
      server.address.c_str(),
      server.port);
    status_.store(ConnectionStatus::CONNECTING_FAILED);
    return;
  }

  C8Log("[remote-service-connection] %s", "rpc thread start");
  thrd_ = std::thread([this,
                       readFd,
                       server]() {
    C8Log("[remote-service-connection] %s", "rpc thread running");
    KjEventListener fdListener;

    kj::Vector<kj::Promise<void>> tasks;

    auto terminate = [this, &server, &fdListener, &tasks, readFd]() {
      C8Log("[remote-service-connection] %s", "rpc thread got terminate signal");
      // Cancel all DNS-SD callbacks and listeners.
      if (this->connectionState_ != nullptr && this->connectionState_->rpcClient.get() != nullptr) {
        C8Log("[remote-service-connection] Disconnecting from \"%s\"\n", server.address.c_str());
      }
      this->connectionState_.reset();

      // Remove the inter-thread event listener and stop listening for
      // events, but defer since this will be called from within the
      // read callback;
      auto defer = kj::evalLater([&fdListener, readFd]() {
                     fdListener.removeFdEvent(readFd);
                     fdListener.stop();
                   })
                     .eagerlyEvaluate(nullptr);
      tasks.add(std::move(defer));
    };

    fdListener.addFdEvent(
      readFd,
      EventFlag::READ | EventFlag::EDGE_TRIGGER | EventFlag::PERSIST,
      [this, readFd, terminate]() {
        this->drainFdAndSendRpcs(readFd, terminate);
      });

    // Create an entry in the connectionState
    this->connectionState_.reset(new ConnectionState());
    connectToServer(server, fdListener.mutableIoContext(), tasks);

    C8Log("[remote-service-connection] %s", "rpc thread waiting for shutdown.");
    fdListener.wait();
    C8Log("[remote-service-connection] %s", "rpc thread ready for join.");
    close(readFd);
  });  // end std::thread
}

void RemoteServiceConnection::disconnect() {
  auto oldStatus = status_.exchange(ConnectionStatus::CLOSED);
  if (oldStatus != ConnectionStatus::CLOSED) {
    C8Log("[remote-service-connection] %s", "disconnect");
    KJ_SYSCALL(write(writeFd_, &terminateChar, sizeof(terminateChar)));
  }
}


RemoteServiceConnection::~RemoteServiceConnection() {
  C8Log("[remote-service-connection] %s", "~RemoteServiceConnection");
  disconnect();
  if (thrd_.joinable()) {
    thrd_.join();
  }
  close(writeFd_);  // don't close before flush
}

void RemoteServiceConnection::send(MutableRootMessage<RemoteServiceRequest> &request) {
  if (requestPair_.fillingMsg != nullptr) {
    C8Log("[remote-service-connection] %s", "dropping request; already buffered.");
    return;
  }

  if (status_.load() != ConnectionStatus::CONNECTED) {
    C8Log("[remote-service-connection] %s", "dropping request; not connected.");
    return;
  }

  // C8Log("[remote-service-connection] %s", "buffering request and notifying network thread");
  requestPair_.fill(request);
  KJ_SYSCALL(write(writeFd_, &updateChar, sizeof(updateChar)));
}

ConstRootMessage<RemoteServiceResponse>& RemoteServiceConnection::receive() {
  if (responsePair_.fillingMsg == nullptr) {
    //C8Log("[remote-service-connection] %s", "no new response, repeating last response.");
    if (responsePair_.drainingMsg == nullptr) {
      C8Log("[remote-service-connection] %s", "initializing default last response.");
      responsePair_.drainingMsg.reset(new ConstRootMessage<RemoteServiceResponse>());
    }
    // No new response, return the old one.
    return *(responsePair_.drainingMsg.get());
  }

  // Invalidate the last returned message.
  responsePair_.drainingMsg.reset();

  // Swap and return the new response.
  responsePair_.swap();
  return *(responsePair_.drainingMsg.get());
}

bool RemoteServiceConnection::canSendRequest() {
  return status() == ConnectionStatus::CONNECTED && requestPair_.fillingMsg == nullptr;
}

// Connect to a new RemoteService server and add it to the map of connections.
void RemoteServiceConnection::connectToServer(
  const RemoteServiceDiscovery::ServiceInfo &server,
  kj::AsyncIoContext &ioContext,
  kj::Vector<kj::Promise<void>> &tasks) {
  C8Log(
    "[remote-service-connection] Connecting to \"%s\" at %s:%d\n",
    server.serviceName.c_str(),
    server.address.c_str(),
    server.port);
  auto createConnection =
    ioContext.provider->getNetwork()
      .parseAddress(server.address, server.port)
      .then([](kj::Own<kj::NetworkAddress> &&address) -> kj::Promise<kj::Own<kj::AsyncIoStream>> {
        C8Log("[remote-service-connection] %s", "connecting - parsed address");
        auto promise = address->connect();
        return promise.attach(kj::mv(address));
      })
      .then([this](kj::Own<kj::AsyncIoStream> &&connection) {
        C8Log("[remote-service-connection] %s", "connecting - create-client");
        this->connectionState_->connection = std::move(connection);
        this->connectionState_->rpcClient =
          kj::heap<capnp::TwoPartyClient>(*this->connectionState_->connection);

        auto bootstrap = this->connectionState_->rpcClient->bootstrap().castAs<RemoteService>();
        this->connectionState_->remoteServiceClient =
          kj::heap<RemoteService::Client>(std::move(bootstrap));
        C8Log("[remote-service-connection] %s", "connecting - connected");
        this->status_.store(RemoteServiceConnection::ConnectionStatus::CONNECTED);
      })
      .eagerlyEvaluate(nullptr);
  tasks.add(std::move(createConnection));
}

// Drain any bytes on the read File descriptor, then send the current queue to each of the connected
//  Rpc servers.
void RemoteServiceConnection::drainFdAndSendRpcs(
  evutil_socket_t readFd,
  std::function<void()> terminate) {
  //C8Log("[remote-service-connection] %s", "Draining Fd for terminate character");
  char readBuffer[4096];
  ssize_t n = 0;
  do {
    KJ_SYSCALL(n = read(readFd, &readBuffer, sizeof(readBuffer)));
    char *endByte = readBuffer + n;
    if (std::find(readBuffer, endByte, terminateChar) != endByte) {
      // End all processing tasks if the terminate char is encountered.
      terminate();
      return;
    }
  } while (n == sizeof(readBuffer));

  if (
    status_.load() != ConnectionStatus::CONNECTED
    || connectionState_ == nullptr
    || connectionState_->rpcClient.get() == nullptr) {
    C8Log("[remote-service-connection] %s", "Not connected; dropping messages.");
    return;
  }


  // If this RPC server is still pending the previous request, return but come back later
  if (requestPair_.drainingMsg != nullptr) {
    //C8Log("[remote-service-connection] %s", "RPC is pending, delaying drain.");
    KJ_SYSCALL(write(this->writeFd_, &updateChar, sizeof(updateChar)));
    return;
  }

  requestPair_.swap();

  if (requestPair_.drainingMsg == nullptr) {
    C8Log("[remote-service-connection] %s", "ERROR!!! send requested but nothing to send.");
    return;
  }

  auto &cap = *connectionState_->remoteServiceClient;
  auto request = cap.logRequest();
  request.setRequest(requestPair_.drainingMsg->reader());

  // C8Log("[remote-service-connection] %s", "Sending request RPC");
  request.send()
    .then([this](capnp::Response<c8::RemoteService::LogResults> &&results) mutable {
      // C8Log("[remote-service-connection] %s ", "Got response for RPC");
      MutableRootMessage<RemoteServiceResponse> m(results.getResponse());
      this->responsePair_.fill(m);
      this->requestPair_.drainingMsg.reset();
    })
    .detach([this](kj::Exception &&e) {
      C8Log("[remote-service-connection] rpc disconnect: %s", e.getDescription().cStr());
      this->connectionState_.reset();
      disconnect();
    });
}

}  // namespace c8
