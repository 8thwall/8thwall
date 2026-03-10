// Copyright (c) 2017 8th Wall, Inc.
// Original Author: Nicholas Butko (nb@8thwall.com)

#include "bzl/inliner/rules2.h"

cc_library {
  hdrs = {
    "remote-streamer.h",
  };
  deps = {
    "//c8:c8-log",
    "//c8:c8-log-proto",
    "//c8/io:capnp-messages",
    "//c8:vector",
    "//c8/pixels:pixel-transforms",
    "//c8/pixels:pixels",
    "//c8/protolog:xr-requests",
    "//c8/protolog:remote-service-connection",
    "//c8/protolog:remote-service-discovery",
    "//c8/protolog/api:log-request.capnp-cc",
    "//c8/protolog/api:remote-request.capnp-cc",
  };
}
cc_end(0x785803e9);

#include <queue>

#include "c8/c8-log-proto.h"
#include "c8/c8-log.h"
#include "c8/pixels/pixel-transforms.h"
#include "c8/pixels/pixels.h"
#include "c8/protolog/xr-requests.h"
#include "reality/engine/logging/remote-streamer.h"

namespace c8 {

namespace {

void expandImagePtrsToImages(
  const RealityRequest::Reader &request, XrRemoteRequest::Builder *record) {
  if (!request.getSensors().getCamera().hasCurrentFrame()) {
    return;
  }

  auto origFrame = request.getSensors().getCamera().getCurrentFrame();

  MutableRootMessage<CameraFrame> imgmsg;
  CameraFrame::Builder frame = imgmsg.builder();

  frame.setVideoTimestampNanos(origFrame.getVideoTimestampNanos());
  frame.setFrameTimestampNanos(origFrame.getFrameTimestampNanos());
  frame.setTimestampNanos(origFrame.getTimestampNanos());

  auto origY = constFrameYPixels(origFrame);
  auto origUV = constFrameUVPixels(origFrame);

  int step = 1;
  while (origY.rows() / step > 640 || origY.cols() / step > 640) {
    ++step;
  }

  if (origY.pixels() != nullptr) {
    int rows = 0;
    int cols = 0;
    int bytesPerRow = 0;
    if (step == 1) {
      rows = origY.rows();
      cols = origY.cols();
      bytesPerRow = origY.rowBytes();
    } else {
      rows = origY.rows() / step;
      cols = origY.cols() / step;
      bytesPerRow = cols;
    }

    auto imBuilder = frame.getImage().getOneOf().initGrayImageData();
    imBuilder.setRows(rows);
    imBuilder.setCols(cols);
    imBuilder.setBytesPerRow(bytesPerRow);
    imBuilder.initUInt8PixelData(rows * bytesPerRow);

    auto destY = frameYPixels(frame);
    downsize(origY, &destY);
  }

  if (origUV.pixels() != nullptr) {
    int rows = 0;
    int cols = 0;
    int bytesPerRow = 0;
    if (step == 1) {
      rows = origUV.rows();
      cols = origUV.cols();
      bytesPerRow = origUV.rowBytes();
    } else {
      rows = origUV.rows() / step;
      cols = origUV.cols() / step;
      bytesPerRow = cols * 2;
    }

    auto imBuilder = frame.getUvImage().getOneOf().initGrayImageData();
    imBuilder.setRows(rows);
    imBuilder.setCols(cols);
    imBuilder.setBytesPerRow(bytesPerRow);
    imBuilder.initUInt8PixelData(rows * bytesPerRow);

    auto destUV = frameUVPixels(frame);
    downsize(origUV, &destUV);
  }

  record->getRealityEngine().getRequest().getSensors().getCamera().initCurrentFrame();
  record->getRealityEngine().getRequest().getSensors().getCamera().setCurrentFrame(frame);
}

void clearImagePtrs(const RealityRequest::Reader &request, XrRemoteRequest::Builder *record) {
  if (!request.getSensors().getCamera().hasCurrentFrame()) {
    return;
  }

  auto origFrame = request.getSensors().getCamera().getCurrentFrame();

  MutableRootMessage<CameraFrame> imgmsg;
  CameraFrame::Builder frame = imgmsg.builder();

  frame.setTimestampNanos(origFrame.getTimestampNanos());

  record->getRealityEngine().getRequest().getSensors().getCamera().initCurrentFrame();
  record->getRealityEngine().getRequest().getSensors().getCamera().setCurrentFrame(frame);
}

}  // namespace

ConstRootMessage<RemoteServiceResponse> RemoteStreamer::FALLBACK_RESPONSE;

RemoteStreamer::~RemoteStreamer() {
  std::lock_guard<std::mutex> lock(implLock_);
  disconnectServer();
  disconnectBrowsing();
}

void RemoteStreamer::pauseConnectionToServer() {
  std::lock_guard<std::mutex> lock(implLock_);
  disconnectServer();
}

void RemoteStreamer::disconnectServer() {
  if (remoteServiceConnection_ != nullptr) {
    C8Log(
      "[remote-streamer] disconnect from %s with status %d",
      connectedServer_ == nullptr ? "nullptr" : connectedServer_->reader().getDisplayName().cStr(),
      remoteServiceConnection_->status());
    remoteServiceConnection_.reset();
  }
  connectedServer_.reset();
}

void RemoteStreamer::stream(
  const RealityRequest::Reader &request,
  const RealityResponse::Reader &response,
  const XrRemoteApp::Reader &xrRemote) {
  std::lock_guard<std::mutex> lock(implLock_);
  if (!connected()) {
    msgQueue_.clear();
    return;
  }

  std::unique_ptr<MutableRootMessage<XrRemoteRequest>> msg(
    new MutableRootMessage<XrRemoteRequest>());
  msgQueue_.push_back(std::move(msg));

  XrRemoteRequest::Builder record = msgQueue_.back()->builder();
  record.getRealityEngine().setRequest(request);
  record.getRealityEngine().setResponse(response);
  record.setXrRemote(xrRemote);

  if (!remoteServiceConnection_->canSendRequest()) {
    clearImagePtrs(request, &record);
    return;
  }

  expandImagePtrsToImages(request, &record);

  MutableRootMessage<c8::RemoteServiceRequest> multimsg;
  RemoteServiceRequest::Builder multirequest = multimsg.builder();
  multirequest.initRecords(msgQueue_.size());
  for (int i = 0; i < msgQueue_.size(); ++i) {
    multirequest.getRecords().setWithCaveats(i, msgQueue_[i]->builder());
  }
  remoteServiceConnection_->send(multimsg);
  msgQueue_.clear();
}

ConstRootMessage<RemoteServiceResponse> &RemoteStreamer::remoteResponse() {
  std::lock_guard<std::mutex> lock(implLock_);
  if (!connected()) {
    return FALLBACK_RESPONSE;
  }
  return remoteServiceConnection_->receive();
}

void RemoteStreamer::pause() {
  C8Log("[remote-streamer] %s", "pause");
  std::lock_guard<std::mutex> lock(implLock_);
  disconnectServer();
}

void RemoteStreamer::resume() {
  // Nothing to do here, the connection is resumed outside the context of the xr engine.
}

void RemoteStreamer::resumeConnectionToServer(ConstRootMessage<XrServer> &server) {
  C8Log("[remote-streamer] %s", "resumeConnectionToServer");
  std::lock_guard<std::mutex> lock(implLock_);
  disconnectServer();
  connectedServer_.reset(
    new ConstRootMessage<XrServer>(server.bytes().begin(), server.bytes().size()));

  auto r = connectedServer_->reader();

  switch (r.getResolveStrategy()) {
    case XrServer::ResolveStrategy::IPV4: {
      RemoteServiceDiscovery::ServiceInfo info;
      info.port = r.getPort();
      info.address = r.getIpV4Address();
      this->remoteServiceConnection_.reset(new RemoteServiceConnection());
      this->remoteServiceConnection_->logToServer(info);
    } break;
    case XrServer::ResolveStrategy::MDNS: {
      if (remoteServiceDiscovery_ == nullptr) {
        C8Log("%s", "Can't resolve mdns addresses while browsing is paused.");
        disconnectServer();
        return;
      }
      remoteServiceDiscovery_->fetchServiceInfo(
        r.getDisplayName().cStr(),
        [this](RemoteServiceDiscovery::ServiceInfo info, RemoteServiceDiscovery *rsd) {
          C8Log(
            "[remote-streamer] fetch finished with address %s:%d", info.address.c_str(), info.port);
          std::lock_guard<std::mutex> lock(this->implLock_);
          if (info.address.empty() || info.port == 0 || connectedServer_ == nullptr) {
            disconnectServer();
            return;
          }
          MutableRootMessage<XrServer> serverWithIp(this->connectedServer_->reader());
          if (strcmp(info.displayName.c_str(), "USB") == 0) {
            serverWithIp.builder().setInterfaceKind(XrServer::InterfaceKind::USB);
          } else {
            serverWithIp.builder().setInterfaceKind(XrServer::InterfaceKind::WIFI);
          }
          serverWithIp.builder().setIpV4Address(info.address);
          serverWithIp.builder().setPort(info.port);
          this->connectedServer_.reset(new ConstRootMessage<XrServer>(serverWithIp));
          this->remoteServiceConnection_.reset(new RemoteServiceConnection());
          this->remoteServiceConnection_->logToServer(info);
        });
    } break;
    default:
      C8Log("%s", "Unsupported resolutions strategy; not connecting.");
      break;
  }
}

ConstRootMessage<XrRemoteConnection> &RemoteStreamer::remoteConnection() {
  std::lock_guard<std::mutex> lock(implLock_);
  MutableRootMessage<XrRemoteConnection> m;
  auto builder = m.builder();

  if (remoteServiceDiscovery_ != nullptr) {
    auto srvs = remoteServiceDiscovery_->getServerList();
#ifdef ANDROID
    // TODO(scott) only addUSB if ADB is active
    bool addUSB = true;
#else
    bool addUSB = false;
#endif
    builder.initAvailableServers((int)srvs.size() + (addUSB ? 1 : 0));

    // Add servers from MDNS.
    int i = 0;
    for (const auto &srv : srvs) {
      auto s = builder.getAvailableServers()[i];
      if (strcmp(srv.c_str(), "USB") == 0) {
        s.setInterfaceKind(XrServer::InterfaceKind::USB);
      } else {
        s.setInterfaceKind(XrServer::InterfaceKind::WIFI);
      }
      s.setResolveStrategy(XrServer::ResolveStrategy::MDNS);
      s.setDisplayName(srv.c_str());
      ++i;
    }

    if (addUSB) {
      auto s = builder.getAvailableServers()[i];
      s.setDisplayName("USB");
      s.setInterfaceKind(XrServer::InterfaceKind::USB);
      s.setResolveStrategy(XrServer::ResolveStrategy::IPV4);
      s.setPort(23285);  // from remote-service-thread.cc
      s.setIpV4Address("127.0.0.1");
    }
  }

  // Add info about connected server.
  if (connectedServer_ != nullptr) {
    builder.getConnectedServer().setServer(connectedServer_->reader());
  }

  // Add info about connection status.
  if (remoteServiceConnection_ != nullptr) {
    switch (remoteServiceConnection_->status()) {
      case RemoteServiceConnection::CONNECTING:
        builder.getConnectedServer().setStatus(XrConnectedServer::XrConnectionStatus::CONNECTING);
        break;
      case RemoteServiceConnection::CONNECTED:
        builder.getConnectedServer().setStatus(XrConnectedServer::XrConnectionStatus::CONNECTED);
        break;
      case RemoteServiceConnection::CLOSED:
        builder.getConnectedServer().setStatus(XrConnectedServer::XrConnectionStatus::CLOSED);
        break;
      case RemoteServiceConnection::CONNECTING_FAILED:
        builder.getConnectedServer().setStatus(
          XrConnectedServer::XrConnectionStatus::CONNECTING_FAILED);
        break;
      default:
        // unspecified.
        break;
    }
  }

  remoteConnection_ = ConstRootMessage<XrRemoteConnection>(m);
  return remoteConnection_;
}

void RemoteStreamer::resumeBrowsingForServers() {
  std::lock_guard<std::mutex> lock(implLock_);
  if (remoteServiceDiscovery_ == nullptr) {
    C8Log("[remote-streamer] %s", "resumeBrowsingForServers");
    remoteServiceDiscovery_.reset(new RemoteServiceDiscovery());
    remoteServiceDiscovery_->startBrowsing();
  }
}

void RemoteStreamer::pauseBrowsingForServers() {
  std::lock_guard<std::mutex> lock(implLock_);
  disconnectBrowsing();
}

void RemoteStreamer::disconnectBrowsing() {
  if (remoteServiceDiscovery_ != nullptr) {
    C8Log("[remote-streamer] %s", "disconnect browsing");
    remoteServiceDiscovery_.reset();
  }
}

};  // namespace c8
