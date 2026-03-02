#include "bzl/inliner/rules2.h"

cc_binary {
  deps = {
    "//c8:c8-log",
    "//c8/protolog/api:log-service.capnp-cc",
    "//c8/protolog:remote-service-connection",
    "//c8/io:capnp-messages",
    "@eigen3",
  };
}
cc_end(0xc2df7a21);

#include <capnp/ez-rpc.h>
#include <Eigen/Core>
#include <Eigen/Geometry>
#include <chrono>
#include <thread>
#include "c8/c8-log.h"
#include "c8/io/capnp-messages.h"
#include "c8/protolog/api/remote-service-interface.capnp.h"
#include "c8/protolog/remote-service-connection.h"
#include "reality/engine/api/base/geo-types.capnp.h"

using namespace c8;
using Eigen::AngleAxis;
using Eigen::Vector3f;

static uint8_t pixelData[] = {
  48,  30,  10,  11,  23,   // Row 0
  231, 255, 247, 255, 255,  // Row 1
  252, 249, 244, 232, 69,   // Row 2
  179, 169, 174, 0,   17,   // Row 3
  36,  72,  69,  31,  43    // Row 4
};

int main(int argc, const char *argv[]) {
  // constexpr char HOST[] = "localhost";
  RemoteServiceDiscovery::ServiceInfo server;
  server.address = "localhost";
  server.port = 23285;
  RemoteServiceConnection connection;
  connection.logToServer(server);

  Vector3f xInit(0.0, 1.0, 0.0);
  Vector3f x = xInit;
  Vector3f dx(0.0, 0.0, 0.0);

  auto angleToTarget = [](const Vector3f &origin, const Vector3f &target) {
    auto originN = origin.normalized();
    auto targetN = target.normalized();
    return AngleAxis<float>(originN.dot(targetN), originN.cross(targetN));
  };

  Vector3f target(1.0, 0.5, 0.0);
  Eigen::Quaternion<float> orientation(angleToTarget(x, target));

  constexpr float MAX_X(0.5f), MAX_DX(1.1f);

  constexpr float FPS = 30.0f;
  constexpr float DX_SCALE = 9.0f;

  C8Log("Sending mock device movements at %0.1f fps.\n", FPS);
  fflush(stdout);

  while (1) {
    // Update the state.
    x = x + dx / FPS;
    orientation = angleToTarget(x, target);

    // Add some noise to the velocity.
    dx = dx + DX_SCALE * Vector3f::Random() / FPS;

    // Clamp to max velocity.
    auto clamp = [](float m, float &v) { v = std::max(-m, std::min(v, m)); };
    clamp(MAX_DX, dx.x());
    clamp(MAX_DX, dx.y());
    clamp(MAX_DX, dx.z());

    // Enforce position borders and reverse velocity to remain in bounds.
    auto maybeReverseDx = [](float x, float origin, float &dx) {
      if (x < origin - MAX_X && dx < .0f) {
        dx = -dx;
      } else if (x > origin + MAX_X && dx > .0f) {
        dx = -dx;
      }
    };
    maybeReverseDx(x.x(), xInit.x(), dx.x());
    maybeReverseDx(x.y(), xInit.y(), dx.y());
    maybeReverseDx(x.z(), xInit.z(), dx.z());

    MutableRootMessage<RemoteServiceRequest> m;
    auto request = m.builder();
    request.initRecords(1);
    auto record = request.getRecords()[0];

    record.getRealityEngine().getRequest().getSensors().getPose().getDevicePose().setW(
      orientation.w());
    record.getRealityEngine().getRequest().getSensors().getPose().getDevicePose().setX(
      orientation.x());
    record.getRealityEngine().getRequest().getSensors().getPose().getDevicePose().setY(
      orientation.y());
    record.getRealityEngine().getRequest().getSensors().getPose().getDevicePose().setZ(
      orientation.z());

    auto f = record.getRealityEngine().getRequest().getSensors().getCamera().getCurrentFrame();
    auto imageBuilder = f.getImage().getOneOf().initGrayImageData();
    imageBuilder.setRows(4);
    imageBuilder.setCols(3);
    imageBuilder.setBytesPerRow(5);
    imageBuilder.initUInt8PixelData(4 * 5);
    std::memcpy(imageBuilder.getUInt8PixelData().begin(), pixelData, 4 * 5);

    connection.send(m);

    std::this_thread::sleep_for(
      std::chrono::duration<int, std::ratio<1, static_cast<int>(FPS)>>(1));
  }

  return 0;
}
