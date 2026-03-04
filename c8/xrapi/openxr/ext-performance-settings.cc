#include "c8/xrapi/openxr/ext-performance-settings.h"

#include "c8/xrapi/openxr/helper.h"

namespace c8 {

ExtPerformanceSettings::ExtPerformanceSettings(XrInstance instance) : m_xrInstance(instance) {
  loadExtPerformanceSettingsFunctions();
}

Vector<const char *> ExtPerformanceSettings::requiredExtensionNames() {
  return {XR_EXT_PERFORMANCE_SETTINGS_EXTENSION_NAME};
}

void ExtPerformanceSettings::loadExtPerformanceSettingsFunctions() {
  loadXrProcOrDie(
    m_xrInstance, "xrPerfSettingsSetPerformanceLevelEXT", &xrPerfSettingsSetPerformanceLevelEXT_);
}

void ExtPerformanceSettings::setCPUPerformanceLevel(
  XrSession session, XrPerfSettingsLevelEXT level) {
  OPENXR_CHECK(
    xrPerfSettingsSetPerformanceLevelEXT_(
      session,
      XR_PERF_SETTINGS_DOMAIN_CPU_EXT,
      level
    ),
    "Failed to set CPU performance level"
  )}

void ExtPerformanceSettings::setGPUPerformanceLevel(
  XrSession session, XrPerfSettingsLevelEXT level) {
  OPENXR_CHECK(
    xrPerfSettingsSetPerformanceLevelEXT_(
      session,
      XR_PERF_SETTINGS_DOMAIN_GPU_EXT,
      level
    ),
    "Failed to set GPU performance level")}

}
