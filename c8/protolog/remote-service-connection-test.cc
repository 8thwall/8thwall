// Copyright (c) 2016 8th Wall, Inc.
// Original Author: Erik Murphy-Chutorian (mc@8thwall.com)

#include "bzl/inliner/rules2.h"

cc_test {
  size = "small";
  deps = {
    ":remote-service-connection",
    "//c8/io:capnp-messages",
    "//c8/protolog/api:remote-service.capnp-cc",
    "@com_google_googletest//:gtest_main",
  };
}
cc_end(0x89a24793);

#include <gtest/gtest.h>

#include "c8/io/capnp-messages.h"
#include "c8/protolog/api/remote-service-interface.capnp.h"
#include "c8/protolog/remote-service-connection.h"

using MutableRemoteServiceRequest = c8::MutableRootMessage<c8::RemoteServiceRequest>;

namespace c8 {

class RemoteServiceConnectionTest : public ::testing::Test {};

TEST_F(RemoteServiceConnectionTest, ConnectAndSend) {
  RemoteServiceDiscovery::ServiceInfo server;

  MutableRemoteServiceRequest recordMsg;
  auto builder = recordMsg.builder();
  builder.initRecords(1);
  auto record = builder.getRecords()[0];

  record.getRealityEngine().getRequest().getSensors().getPose().getDevicePose().setW(1.0);
  record.getRealityEngine().getRequest().getSensors().getPose().getDevicePose().setX(2.0);
  record.getRealityEngine().getRequest().getSensors().getPose().getDevicePose().setY(3.0);
  record.getRealityEngine().getRequest().getSensors().getPose().getDevicePose().setZ(4.0);

  RemoteServiceConnection connection;
  connection.logToServer(server);
  connection.send(recordMsg);
}

}  // namespace c8
