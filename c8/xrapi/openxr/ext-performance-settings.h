#pragma once

#include "c8/vector.h"
#include "c8/xrapi/openxr/openxr.h"

namespace c8 {

class ExtPerformanceSettings {
public:
  ExtPerformanceSettings(XrInstance instance);

  static Vector<const char *> requiredExtensionNames();

  void setCPUPerformanceLevel(XrSession session, XrPerfSettingsLevelEXT level);

  void setGPUPerformanceLevel(XrSession session, XrPerfSettingsLevelEXT level);

private:
  void loadExtPerformanceSettingsFunctions();

  XrInstance m_xrInstance;

  PFN_xrPerfSettingsSetPerformanceLevelEXT xrPerfSettingsSetPerformanceLevelEXT_ = nullptr;
};

}
