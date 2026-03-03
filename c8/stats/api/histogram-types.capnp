@0x9fd04a54d2da9472;

using Cxx = import "/capnp/c++.capnp";
$Cxx.namespace("c8");

using Java = import "/capnp/java.capnp";
$Java.package("com.the8thwall.c8.stats.api");
$Java.outerClassname("HistogramTypes");  # Must match this file's name!

struct DistributionStats @0xdc7640de3b5b0520 {
  meanStats @0 :MeanStats;
  positiveHistogram @1 :PositiveHistogram;
  percentageHistogram @2 :PercentageHistogram;
}

# A positive histogram with doubling buckets.
struct PositiveHistogram @0xa6d8985830c22206 {
  bucket0 @0 :UInt64;
  bucket1 @1 :UInt64;
  bucket2To3 @2 :UInt64;
  bucket4To7 @3 :UInt64;
  bucket8To15 @4 :UInt64;
  bucket16To31 @5 :UInt64;
  bucket32To63 @6 :UInt64;
  bucket64To127 @7 :UInt64;
  bucket128To255 @8 :UInt64;
  bucket256To511 @9 :UInt64;
  bucket512To1023 @10 :UInt64;
  bucket1024To2047 @11 :UInt64;
  bucket2048To4095 @12 :UInt64;
  bucket4096To8191 @13 :UInt64;
  bucket8192To16383 @14 :UInt64;
  bucket16384To32767 @15 :UInt64;
  bucket32768To65535 @16 :UInt64;
  bucket65536To131071 @17 :UInt64;
  bucket131072To262143 @18 :UInt64;
  bucket262144To524287 @19 :UInt64;
  bucket524288To1048575 @20 :UInt64;
  bucket1048576To2097151 @21 :UInt64;
  bucket2097152To4194303 @22 :UInt64;
  bucket4194304To8388607 @23 :UInt64;
  bucket8388608To16777215 @24 :UInt64;
  bucket16777216To33554431 @25 :UInt64;
  bucket33554432To67108863 @26 :UInt64;
  bucket67108864To134217727 @27 :UInt64;
  bucket134217728To268435455 @28 :UInt64;
  bucket268435456To536870911 @29 :UInt64;
  bucket536870912To1073741823 @30 :UInt64;
  bucket1073741824ToInf @31 :UInt64;
}

# A histogram where each event corresponds to a ratio between zero and one. The
# logistic stepping puts more resolution on values close to zero and one. This
# Histogram uses 6 decimal places of precsion, so 0000001 is 0.000001, and
# 0999999 is 0.999999.
struct PercentageHistogram @0xa987e131351b3565 {
  bucket0000000 @0 :UInt64;
  bucket0000001 @1 :UInt64;
  bucket0000002To0000005 @2 :UInt64;
  bucket0000006To0000016 @3 :UInt64;
  bucket0000017To0000044 @4 :UInt64;
  bucket0000045To0000122 @5 :UInt64;
  bucket0000123To0000334 @6 :UInt64;
  bucket0000335To0000910 @7 :UInt64;
  bucket0000911To0002472 @8 :UInt64;
  bucket0002473To0006692 @9 :UInt64;
  bucket0006693To0017985 @10 :UInt64;
  bucket0017986To0047425 @11 :UInt64;
  bucket0047426To0119202 @12 :UInt64;
  bucket0119203To0268940 @13 :UInt64;
  bucket0268941To0499999 @14 :UInt64;
  bucket0500000To0731059 @15 :UInt64;
  bucket0731060To0880797 @16 :UInt64;
  bucket0880798To0952574 @17 :UInt64;
  bucket0952575To0982014 @18 :UInt64;
  bucket0982015To0993307 @19 :UInt64;
  bucket0993308To0997527 @20 :UInt64;
  bucket0997528To0999089 @21 :UInt64;
  bucket0999090To0999665 @22 :UInt64;
  bucket0999666To0999877 @23 :UInt64;
  bucket0999878To0999955 @24 :UInt64;
  bucket0999956To0999983 @25 :UInt64;
  bucket0999984To0999994 @26 :UInt64;
  bucket0999995To0999998 @27 :UInt64;
  bucket0999999 @28 :UInt64;
  bucket1000000 @29 :UInt64;
}

# Sufficient statistics for mean, variance, and geometric mean.
struct MeanStats @0x9242cef725d2bc56 {
  numDataPoints @0 :UInt64;
  sum @1 :Int64;
  sumOfSquares @2 :UInt64;
  sumOfLogs @3 :Float64;
}
