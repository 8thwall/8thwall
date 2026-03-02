// Copyright (c) 2018 8th Wall, Inc.
// Original Author: Nicholas Butko (nb@8thwall.com)

#include "bzl/inliner/rules.h"

cc_binary {
  deps = {
    "//bzl/inliner:rules",
    "//c8:c8-log",
    "//c8:c8-log-proto",
    "//c8/pixels:pixels",
    "//c8/protolog/api:remote-service.capnp-cc",
  };
}

#include <capnp/ez-rpc.h>
#include <chrono>
#include <iostream>
#include <thread>
#include "c8/c8-log-proto.h"
#include "c8/c8-log.h"
#include "c8/pixels/pixels.h"
#include "c8/protolog/api/remote-service-interface.capnp.h"

using namespace c8;

static uint8_t pixelData[] = {
  48,  30,  10,  11,  23,   // Row 0
  231, 255, 247, 255, 255,  // Row 1
  252, 249, 244, 232, 69,   // Row 2
  179, 169, 174, 0,   17,   // Row 3
  36,  72,  69,  31,  43    // Row 4
};

int main(int argc, char *argv[]) {
  String host = "127.0.0.1";
  uint16_t port = 23285;

  // Set up the EzRpcClient, connecting to the server on port.
  capnp::EzRpcClient client(host.c_str(), port);
  auto &waitScope = client.getWaitScope();

  // Request the bootstrap capability from the server.
  RemoteService::Client cap = client.getMain<RemoteService>();

  auto request = cap.logRequest();
  auto b = request.getRequest();

  // Make a call to the capability.
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

  C8Log("%s", "Sending request:");
  C8LogCapnpMessage(request.getRequest().asReader());
  // Wait for the result.  This is the only line that blocks.
  auto promise = request.send();
  auto response = promise.wait(waitScope);

  // All done.
  C8Log("%s", "Got response:");
  C8LogCapnpMessage(response.getResponse());
  return 0;
}
