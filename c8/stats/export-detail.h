#include "c8/map.h"
#include "c8/set.h"
#include "c8/stats/api/detail.capnp.h"
#include "c8/stats/logging-context.h"
#include "c8/string.h"
#include "c8/vector.h"

namespace c8 {
HashMap<String, uint64_t> computeFlamegraphValues(const LoggingDetail::Reader &reader);

// Get text that can be interpreted by the flamegraph.pl utility (brew install flamegraph) and used
// for generating an svg.
String flamegraphText(LoggingContext *lc = nullptr);

}
