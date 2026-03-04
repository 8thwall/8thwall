#include <emscripten.h>

#include <memory>
#include <nlohmann/json.hpp>

#include "c8/c8-log.h"
#include "c8/map.h"
#include "c8/set.h"
#include "c8/pixels/render/gesture-detector.h"
#include "c8/stats/scope-timer.h"
#include "c8/symbol-visibility.h"

using namespace c8;

namespace {

class GestureDetectorWrapper {
public:
  static uint32_t create() {
    static uint32_t nextWrapper_ = 0;
    auto id = nextWrapper_++;
    wrappers_[id].reset(new GestureDetectorWrapper());
    return id;
  }

  static void destroy(uint32_t wid) { wrappers_.erase(wid); }

  static uint32_t addListener(uint32_t wid, const String &event) {
    return wrappers_[wid]->addListener(event);
  }

  static void removeListener(uint32_t wid, const String &event, uint32_t eid) {
    wrappers_[wid]->removeListener(event, eid);
  }

  static Vector<std::tuple<uint32_t, String, TouchState>> observe(
    uint32_t wid, const TouchEvent &e) {
    return wrappers_[wid]->observe(e);
  }

private:
  GestureDetectorWrapper() = default;

  static TreeMap<uint32_t, std::unique_ptr<GestureDetectorWrapper>> wrappers_;
  GestureDetector gestureDetector_;
  uint32_t nextId_ = 0;
  TreeMap<String, TreeSet<uint32_t>> listeners_;
  Vector<std::tuple<uint32_t, String, TouchState>> events_;
  GestureDetector::GestureHandler listen_ = [this](const String &event, const TouchState &state) {
    for (auto listener : this->listeners_[event]) {
      events_.push_back({listener, event, state});
    }
  };

  uint32_t addListener(const String &event) {    
    auto &listeners = listeners_[event];
    if (listeners.empty()) {
      gestureDetector_.addListener(event, &listen_);
    }

    auto nextId = nextId_++;
    listeners.insert(nextId);

    return nextId;
  }

  void removeListener(const String &event, uint32_t id) {
    auto &listeners = listeners_[event];
    listeners.erase(id);
    if (listeners.empty()) {
      gestureDetector_.removeListener(event, &listen_);
    }
  }

  Vector<std::tuple<uint32_t, String, TouchState>> observe(const TouchEvent &e) {
    gestureDetector_.observe(e);
    Vector<std::tuple<uint32_t, String, TouchState>> events;
    events.swap(events_);
    return events;
  }
};

struct StaticData {
  String gestureEvents = "";
};

StaticData &data() {
  static StaticData data;
  return data;
}

TreeMap<uint32_t, std::unique_ptr<GestureDetectorWrapper>> GestureDetectorWrapper::wrappers_ = {};

}  // namespace

extern "C" {

C8_PUBLIC
uint32_t c8EmAsm_addListener(uint32_t wid, const char *event) {
  auto eid = GestureDetectorWrapper::addListener(wid, event);
  return eid;
}

C8_PUBLIC
void c8EmAsm_removeListener(uint32_t wid, const char *event, uint32_t eid) {
  GestureDetectorWrapper::removeListener(wid, event, eid);
}

C8_PUBLIC
uint32_t c8EmAsm_createGestureDetector() {
  return GestureDetectorWrapper::create();
}

C8_PUBLIC
void c8EmAsm_destroyGestureDetector(uint32_t wid) {
  GestureDetectorWrapper::destroy(wid);
}

C8_PUBLIC
void c8EmAsm_observe(uint32_t wid, const char *touchJson) {
  auto json = nlohmann::json::parse(touchJson);
  TouchEvent e;
  e.timeMillis = json["timeMillis"].get<double>();
  e.count = json["count"].get<int>();
  for (int i = 0; i < e.count; ++i) {
    auto &je = json["pos"][i];
    e.pos.push_back({
      static_cast<float>(je["x"].get<float>()),
      static_cast<float>(je["y"].get<float>()),
      static_cast<float>(je["rawX"].get<float>()),
      static_cast<float>(je["rawY"].get<float>()),
    });
  }
  e.viewWidth = json["viewWidth"].get<int>();
  e.viewHeight = json["viewHeight"].get<int>();
  e.screenWidth = json["screenWidth"].get<int>();
  e.screenHeight = json["screenHeight"].get<int>();
  auto events = GestureDetectorWrapper::observe(wid, e);
  nlohmann::json eventsJson = nlohmann::json::array();
  for (auto &event : events) {
    auto [id, name, state] = event;
    eventsJson.push_back({
      {"id", id},
      {"event", name},
      {"state", {
        {"touchCount", state.touchCount},
        {"positionRaw", {
          {"x", state.positionRaw.x()},
          {"y", state.positionRaw.y()},
        }},
        {"position", {
          {"x", state.position.x()},
          {"y", state.position.y()},
        }},
        {"velocity", {
          {"x", state.velocity.x()},
          {"y", state.velocity.y()},
        }},
        {"positionClip", {
          {"x", state.positionClip.x()},
          {"y", state.positionClip.y()},
        }},
        {"normalizedViewSize", {
          {"x", state.normalizedViewSize.x()},
          {"y", state.normalizedViewSize.y()},
        }},
        {"spread", state.spread},
        {"touchMillis", state.touchMillis},
        {"startMillis", state.startMillis},
        {"startPosition", {
          {"x", state.startPosition.x()},
          {"y", state.startPosition.y()},
        }},
        {"startSpread", state.startSpread},
        {"positionChange", {
          {"x", state.positionChange.x()},
          {"y", state.positionChange.y()},
        }},
        {"spreadChange", state.spreadChange},
        {"startRotationRadian", state.startRotationRadian},
        {"rotationRadianChange", state.rotationRadianChange},
      }},
    });
  }
  nlohmann::json resultJson;
  resultJson["events"] = eventsJson;
  data().gestureEvents = resultJson.dump();
  EM_ASM_(
    {
      window._browserWasm = {};
      window._browserWasm.events = UTF8ToString($0, $1);
    },
    data().gestureEvents.c_str(),
    data().gestureEvents.size());
}

}  // EXTERN "C"
