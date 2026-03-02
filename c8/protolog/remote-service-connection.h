// Copyright (c) 2017 8th Wall, Inc.
// Original Author: Erik Murphy-Chutorian (mc@8thwall.com)
//
// C-wrapper functions for client code to communicate with the RemoteService server.

#pragma once

#include <capnp/message.h>
#include <capnp/rpc-twoparty.h>
#include <event2/event.h>
#include <kj/async-io.h>
#include <kj/async-unix.h>
#include <kj/async.h>
#include <kj/common.h>
#include <list>
#include <memory>
#include <mutex>
#include <queue>
#include <string>
#include <thread>
#include "c8/exceptions.h"
#include "c8/io/capnp-messages.h"
#include "c8/protolog/api/remote-request.capnp.h"
#include "c8/protolog/api/remote-service-interface.capnp.h"
#include "c8/protolog/remote-service-discovery.h"
#include "c8/string.h"
#include "c8/vector.h"

namespace c8 {

struct ConnectionState {
  kj::Own<kj::AsyncIoStream> connection;
  kj::Own<capnp::TwoPartyClient> rpcClient;
  kj::Own<RemoteService::Client> remoteServiceClient;

  ConnectionState() = default;
  ~ConnectionState() = default;

  ConnectionState(ConnectionState &&rhs) = default;
  ConnectionState &operator=(ConnectionState &&rhs) = default;

  ConnectionState(const ConnectionState &) = delete;
  ConnectionState &operator=(const ConnectionState &) = delete;
};

template <typename T>
struct MessagePair {
  std::unique_ptr<ConstRootMessage<T>> fillingMsg;
  std::unique_ptr<ConstRootMessage<T>> drainingMsg;

  void swap() {
    std::lock_guard<std::mutex> lock(fillGuard);
    if (drainingMsg != nullptr) {
      C8_THROW("MessagePair has not been drained.");
    }
    fillingMsg.swap(drainingMsg);
  }

  // Swap is called from the draining thread and doesn't need to be locked with draining operations.
  void fill(MutableRootMessage<T> &m) {
    std::lock_guard<std::mutex> lock(fillGuard);
    fillingMsg.reset(new ConstRootMessage<T>(m));
  }

private:
  std::mutex fillGuard;  // TODO(nb): lockless for efficiency
};

using RequestMessagePair = MessagePair<RemoteServiceRequest>;
using ResponseMessagePair = MessagePair<RemoteServiceResponse>;

class RemoteServiceConnection {
public:
  enum ConnectionStatus {
    UNSPECIFIED = 0,
    CONNECTING = 1,
    CONNECTED = 2,
    CLOSED = 3,
    CONNECTING_FAILED = 4,
  };

  RemoteServiceConnection();

  ~RemoteServiceConnection();

  // Open connection and initiate logging.
  void logToServer(RemoteServiceDiscovery::ServiceInfo server);

  // Send request message. If canSendRequest() is false, this method returns immediately without
  // sending. If canSendRequest() is true, this method copies the request and buffers it to be sent
  // on a different thread.
  void send(MutableRootMessage<RemoteServiceRequest> &request);

  // Check whether it is ok to call "send" without data loss.
  bool canSendRequest();

  // Receive the most recent response. The received object is valid until the next call to receive,
  // or until this RemoteServiceConnection is destroyed.
  ConstRootMessage<RemoteServiceResponse>& receive();

  // Check the connection status.
  ConnectionStatus status() { return status_.load(); }

private:
  evutil_socket_t writeFd_ = 0;
  RequestMessagePair requestPair_;
  ResponseMessagePair responsePair_;
  std::unique_ptr<ConnectionState> connectionState_;

  std::thread thrd_;
  std::atomic<ConnectionStatus> status_;

  void connectToServer(
    const RemoteServiceDiscovery::ServiceInfo &server,
    kj::AsyncIoContext &ioContext,
    kj::Vector<kj::Promise<void>> &tasks);

  void drainFdAndSendRpcs(
    evutil_socket_t readFd,
    std::function<void()> terminate);

  void disconnect();
};

}  // namespace c8
