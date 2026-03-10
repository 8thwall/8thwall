@0xbbc769de6d598a3f;

using import "./histogram-types.capnp".DistributionStats;

using Cxx = import "/capnp/c++.capnp";
$Cxx.namespace("c8");

using Java = import "/capnp/java.capnp";
$Java.package("com.the8thwall.c8.stats.api");
$Java.outerClassname("Summary");  # Must match this file's name!

struct LoggingSummary @0xf86c9f80ccdfd787 {
    events @0 :List(EventSummary);
}

struct EventSummary @0x994045a08723a923 {
  eventName @0 :Text;
  eventCount @1 :UInt64;

  # How long the event took, if relevant.
  latencyMicros @2 :DistributionStats;

  # A positive magnitude associated with each event, and optionally a total.
  positiveCount @3 :DistributionStats;
  ofTotalPositiveCount @4 :DistributionStats;
  percentPositiveCount @5 :DistributionStats;
}
