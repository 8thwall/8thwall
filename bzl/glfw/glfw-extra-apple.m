// Copyright (c) 2023 8th Wall, Inc.
// Original Author: Erik Murphy-Chutorian (mc@nianticlabs.com)

#include "glfw-extra-apple.h"

#include <Cocoa/Cocoa.h>
#include <QuartzCore/CoreAnimation.h>

void *nc_getMetalLayer(void *window) {
  id metalLayer = NULL;
  NSWindow *nsWindow = (NSWindow *)window;
  [nsWindow.contentView setWantsLayer:YES];
  metalLayer = [CAMetalLayer layer];
  [nsWindow.contentView setLayer:metalLayer];
  return metalLayer;
}
