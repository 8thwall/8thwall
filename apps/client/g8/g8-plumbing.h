#pragma once

#include "c8/git/g8-api.capnp.h"
#include "c8/io/capnp-messages.h"
#include "c8/string.h"

c8::ConstRootMessage<c8::G8ChangesetResponse> listChangesets(
  const MainContext &ctx, const bool findRenames);

c8::ConstRootMessage<c8::G8ChangesetResponse> listChangesets(
  const MainContext &ctx, const bool findRenames, const c8::String &client);
