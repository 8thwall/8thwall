// Copyright (c) 2017 8th Wall, Inc.
// Original Author: Nicholas Butko (nb@8thwall.com)

#pragma once

#include <list>
#include <memory>
#include <mutex>
#include <string>

#include "c8/io/capnp-messages.h"
#include "c8/protolog/api/log-request.capnp.h"
#include "c8/protolog/api/remote-request.capnp.h"
#include "c8/protolog/remote-service-connection.h"
#include "c8/protolog/remote-service-discovery.h"
#include "c8/string.h"
#include "c8/vector.h"

namespace c8 {

class RemoteStreamer {
public:
  void stream(
    const RealityRequest::Reader &request,
    const RealityResponse::Reader &response,
    const XrRemoteApp::Reader &xrRemote);

  ConstRootMessage<RemoteServiceResponse> &remoteResponse();
  ConstRootMessage<XrRemoteConnection> &remoteConnection();

  void resumeBrowsingForServers();
  void pauseBrowsingForServers();

  void resumeConnectionToServer(ConstRootMessage<XrServer> &server);
  void pauseConnectionToServer();

  void pause();
  void resume();
  Vector<String> getServerList();
  void setWifiInterfaceIndex(int index) { remoteServiceDiscovery_->setWifiInterfaceIndex(index); }

  // Default constructor.
  RemoteStreamer() = default;
  ~RemoteStreamer();

  // Disallow move constructors, since members have inaccessable move constructors.
  RemoteStreamer(RemoteStreamer &&) = delete;
  RemoteStreamer &operator=(RemoteStreamer &&) = delete;

  // Disallow copying.
  RemoteStreamer(const RemoteStreamer &) = delete;
  RemoteStreamer &operator=(const RemoteStreamer &) = delete;

private:
  std::mutex implLock_;
  std::unique_ptr<RemoteServiceDiscovery> remoteServiceDiscovery_;
  std::unique_ptr<RemoteServiceConnection> remoteServiceConnection_;
  Vector<std::unique_ptr<MutableRootMessage<XrRemoteRequest>>> msgQueue_;

  std::unique_ptr<ConstRootMessage<XrServer>> connectedServer_;
  ConstRootMessage<XrRemoteConnection> remoteConnection_;

  // An empty fallback response for when the connection is not established.
  static ConstRootMessage<RemoteServiceResponse> FALLBACK_RESPONSE;

  bool connected() {
    return remoteServiceConnection_ != nullptr
      && remoteServiceConnection_->status() == RemoteServiceConnection::ConnectionStatus::CONNECTED;
  }

  void disconnectServer();
  void disconnectBrowsing();
};

}  // namespace c8
