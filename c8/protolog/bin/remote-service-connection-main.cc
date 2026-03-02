// Copyright (c) 2018 8th Wall, Inc.
// Original Author: Nicholas Butko (nb@8thwall.com)

#include "bzl/inliner/rules2.h"

cc_binary {
  deps = {
    "//c8:c8-log",
    "//c8/pixels:pixels",
    "//c8/protolog:remote-service-connection",
    "//c8/protolog:remote-service-discovery",
    "//c8/protolog:xr-requests",
  };
}
cc_end(0x15d572ed);

#include <chrono>
#include <iostream>
#include <thread>
#include "c8/c8-log.h"
#include "c8/pixels/pixels.h"
#include "c8/protolog/remote-service-connection.h"
#include "c8/protolog/remote-service-discovery.h"
#include "c8/protolog/xr-requests.h"

using namespace c8;

enum AppState {
  PICKING_SERVER = 0,
  WAITING_FOR_CONNECT = 1,
  CONNECTED = 2,
  BROKEN = 3,
};

static AppState state;
static RemoteServiceDiscovery rsd;
static std::unique_ptr<RemoteServiceConnection> con(new RemoteServiceConnection());
constexpr float FPS = 100.0f;

static uint8_t pixelData[] = {
  48,  30,  10,  11,  23,   // Row 0
  231, 255, 247, 255, 255,  // Row 1
  252, 249, 244, 232, 69,   // Row 2
  179, 169, 174, 0,   17,   // Row 3
  36,  72,  69,  31,  43    // Row 4
};

void printServerListPrompt(AppState state, RemoteServiceDiscovery &rsd) {
  if (state != AppState::PICKING_SERVER) {
    return;
  }
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

bool handlePickingState() {
  printServerListPrompt(state, rsd);
  int choice;
  std::cin >> choice;
  if (choice < 0) {
    return true;
  }

  auto servers = rsd.getServerList();
  if (choice >= servers.size()) {
    C8Log("%s", "Invalid choice.");
    return false;
  }

  state = WAITING_FOR_CONNECT;

  rsd.fetchServiceInfo(
    servers[choice], [](RemoteServiceDiscovery::ServiceInfo info, RemoteServiceDiscovery *rsd) {
      con->logToServer(info);
    });
  return false;
}

bool handleWaitingForConnectState() {
  switch (con->status()) {
    case RemoteServiceConnection::ConnectionStatus::UNSPECIFIED:
      C8Log("%s", "Waiting for connect, status = UNSPECIFIED");
      break;
    case RemoteServiceConnection::ConnectionStatus::CONNECTING:
      C8Log("%s", "Waiting for connect, status = CONNECTING");
      break;
    case RemoteServiceConnection::ConnectionStatus::CONNECTED:
      C8Log("%s", "Waiting for connect, status = CONNECTED");
      state = AppState::CONNECTED;
      break;
    case RemoteServiceConnection::ConnectionStatus::CLOSED:
      C8Log("%s", "Waiting for connect, status = CLOSED");
      state = AppState::BROKEN;
      break;
    case RemoteServiceConnection::ConnectionStatus::CONNECTING_FAILED:
      C8Log("%s", "Waiting for connect, status = CONNECTING_FAILED");
      state = AppState::BROKEN;
      break;
    default:
      C8Log("%s", "Waiting for connect, BAD STATUS");
      state = AppState::BROKEN;
      break;
  }
  std::this_thread::sleep_for(std::chrono::duration<int, std::ratio<1, static_cast<int>(FPS)>>(1));
  return false;
}

void reset() {
  con.reset(new RemoteServiceConnection());
  state = AppState::PICKING_SERVER;
}

bool handleConnectedState() {
  C8Log("%s", "Connected; Enter a number of requests to send, or -1 to disconnect.");

  if (con->status() != RemoteServiceConnection::ConnectionStatus::CONNECTED) {
    C8Log("%s", "Connection interrupted; resetting.");
    reset();
    return false;
  }

  int choice;
  std::cin >> choice;
  if (choice < 0) {
    reset();
    return false;
  }
  while (choice > 0) {
    if (con->status() != RemoteServiceConnection::ConnectionStatus::CONNECTED) {
      C8Log("%s", "Connection interrupted; resetting.");
      reset();
      return false;
    }
    C8Log("Sending %d requests.", choice);
    MutableRootMessage<RemoteServiceRequest> m;
    auto b = m.builder();
    b.initRecords(1);

    YPlanePixels srcY(4, 3, 5, pixelData);
    UVPlanePixels srcUV(0, 0, 0, nullptr);

    auto f =
      b.getRecords()[0].getRealityEngine().getRequest().getSensors().getCamera().getCurrentFrame();

    GrayImageData::Builder imageBuilder = f.getImage().getOneOf().initGrayImageData();
    imageBuilder.setRows(4);
    imageBuilder.setCols(3);
    imageBuilder.setBytesPerRow(5);
    imageBuilder.initUInt8PixelData(4 * 5);
    std::memcpy(imageBuilder.getUInt8PixelData().begin(), pixelData, 4 * 5);

    con->send(m);
    std::this_thread::sleep_for(
      std::chrono::duration<int, std::ratio<1, static_cast<int>(FPS)>>(1));
    --choice;
  }
  return false;
}

bool handleBrokenState() {
  C8Log("%s", "Broken connection, resetting.");
  reset();
  return false;
}

int main(int argc, char *argv[]) {
  state = AppState::PICKING_SERVER;

  rsd.setServerListUpdatedCallback([](RemoteServiceDiscovery *rsd) {
    C8Log("[remote-service-discovery-main] %s", "Server list updated.");
    printServerListPrompt(state, *rsd);
  });

  C8Log("[remote-service-discovery-main] %s", "Starting browsing.");
  rsd.startBrowsing();
  C8Log("[remote-service-discovery-main] %s", "Browsing started.");

  printServerListPrompt(state, rsd);

  bool done = false;
  while (!done) {
    switch (state) {
      case PICKING_SERVER:
        done = handlePickingState();
        break;
      case WAITING_FOR_CONNECT:
        done = handleWaitingForConnectState();
        break;
      case CONNECTED:
        done = handleConnectedState();
        break;
      case BROKEN:
        done = handleBrokenState();
        break;
      default:
        C8Log("Unexpected state %d", state);
        done = true;
        break;
    }
  }

  return 0;
}
