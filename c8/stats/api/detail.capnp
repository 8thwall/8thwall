@0xefdb6ab6dfeb6e8b;

using Cxx = import "/capnp/c++.capnp";
$Cxx.namespace("c8");

using Java = import "/capnp/java.capnp";
$Java.package("com.the8thwall.c8.stats.api");
$Java.outerClassname("Detail");  # Must match this file's name!

struct LoggingDetail @0xdc532590a8a1321b {
  events @0 :List(EventDetail);
}

struct EventDetail @0xcfde2890380dfea9 {
  eventName @0 :Text;
  eventId @1 :Text;
  parentId @2 :Text;

  # For events with a duration.
  startTimeMicros @3 :UInt64;
  endTimeMicros @4 :UInt64;

  # For events with a magnitude associated with them. ofTotal may be optionally supplied to indicate
  # a percentage value associated with this event.
  positiveCount @5 :UInt64;
  ofTotalPositiveCount @6 :UInt64;
}

struct LoggingTraces @0xb1ae3ad3ba8802c5 {
  traces @0: List(LoggingDetail);
}
