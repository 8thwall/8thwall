#pragma once

#include "apps/g8/g8-helpers.h"
#include "c8/string.h"

void keepHouseImpl(MainContext &ctx);

void inspectImpl(MainContext &ctx, const c8::String &inspectPoint, const c8::String &inspectRegex);

void cloneImpl(
  MainContext &ctx,
  const c8::String &name,
  const c8::String &host,
  const c8::String &dir,
  const c8::String &cloneCheckoutId);

void newClientImpl(MainContext &ctx, const c8::Vector<c8::String> &clientNames);
void presubmitImpl(MainContext &ctx, c8::String changesetId);
void syncMasterImpl(MainContext &ctx);
void lsImpl(MainContext &ctx, const LsOptions &opts, const c8::Vector<c8::String> &paths);
