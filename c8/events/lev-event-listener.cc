// Copyright (c) 2018 8th Wall, Inc.
// Original Author: Dat Chu (dat@8thwall.com)

#include "bzl/inliner/rules.h"

cc_library {
  hdrs = {
    "lev-event-listener.h",
  };
  deps = {
    ":event-listener",
    "//bzl/inliner:rules",
    "//c8:c8-log",
    "//c8:exceptions",
    "//c8:map",
    "@libevent//:event",
  };
}
#include <event2/event.h>

#include "c8/c8-log.h"
#include "c8/events/lev-event-listener.h"
#include "c8/exceptions.h"

namespace c8 {

namespace {
#ifdef _WIN32
constexpr auto SOCKPAIR_TYPE = AF_INET;
#else
constexpr auto SOCKPAIR_TYPE = AF_UNIX;
#endif

std::map<EventFlags, short> EventFlagToLevFlag = {{EventFlag::READ, EV_READ},
                                                  {EventFlag::WRITE, EV_WRITE},
                                                  {EventFlag::EDGE_TRIGGER, EV_ET},
                                                  {EventFlag::PERSIST, EV_PERSIST}};
}

LevEventListener::LevEventListener() {
#ifdef _WIN32
  WSADATA WsaData;
  int startupCode = WSAStartup(0x0201, &WsaData);
  if (startupCode != 0) {
    C8_THROW(
      "Unable to initialize Windows Winsock. Error code = " + std::to_string(startupCode));
  }
#endif

  eventBase_ = event_base_new();
  if (eventBase_ == nullptr) {
    C8_THROW("Could not initialize libevent");
  }
}

LevEventListener::~LevEventListener() {
  for (const auto &fdToEvent : fdIntToEvent_) {
    event_free(fdToEvent.second->event);
  }
  fdIntToEvent_.clear();
  if (eventBase_ != nullptr) {
    event_base_free(eventBase_);
  }

#ifdef _WIN32
  C8Log("%s", "Destroy WinSock resource");
  int cleanupCode = WSACleanup();
  if (cleanupCode != 0) {
    C8Log("Unable to cleanup WinSock resource. Error code = %d", cleanupCode);
  }
#endif
}

short LevEventListener::convertToLevFlags(EventFlags flags) {
  short evFlags = 0;
  for (auto const &fMap : EventFlagToLevFlag) {
    if (flags & fMap.first) {
      evFlags |= fMap.second;
    }
  }
  return evFlags;
}

void eventNewCb(evutil_socket_t fd, short what, void *arg) {
  if (!(what & EV_TIMEOUT)) {
    std::function<void()> *callback = static_cast<std::function<void()> *>(arg);
    (*callback)();
  }
}

/**
 * @note(dat): Does not callback on a TIMEOUT event
 */
void LevEventListener::addFdEvent(FdInt fd, EventFlags flags, std::function<void()> callback) {
  short evFlags = convertToLevFlags(flags);

  if (!((evFlags & EventFlag::READ) || (evFlags & EventFlag::WRITE))) {
    C8_THROW("EventFlags have neither READ nor WRITE");
  }
  std::unique_ptr<EventCB> evCB(new EventCB());
  evCB->callback = callback;  // Making a copy of the callback
  event *ev = event_new(eventBase_, fd, evFlags, eventNewCb, &(evCB->callback));
  evCB->event = ev;
  if (ev == nullptr) {
    C8_THROW("Unable to create a READ event");
  }
  event_add(ev, nullptr);
  fdIntToEvent_.insert(std::make_pair(fd, std::move(evCB)));
}

void LevEventListener::removeFdEvent(FdInt fd) {
  const EventCB &evCB = *fdIntToEvent_.at(fd);
  event *ev = evCB.event;
  if (ev != nullptr) {
    fdIntToEvent_.erase(fd);
    event_free(ev);
  }
}

void LevEventListener::wait() {
  if (eventBase_ == nullptr) {
    throw new RuntimeError("You cannot wait without an eventBase");
  }

  event_base_dispatch(eventBase_);
}

void LevEventListener::stop() {
  if (eventBase_ != nullptr) {
    event_base_loopexit(eventBase_, nullptr);  // finish the current callbacks then stop
  } else {
    throw new RuntimeError("You cannot stop without an eventBase");
  }
}

int LevEventListener::createSocketPair(evutil_socket_t *sockets) {
  return evutil_socketpair(SOCKPAIR_TYPE, SOCK_STREAM, 0, sockets);
}

}  // namespace c8
