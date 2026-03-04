#include "meta-virtual-keyboard.h"

#include "c8/xrapi/openxr/helper.h"

namespace c8 {

MetaVirtualKeyboard::MetaVirtualKeyboard(XrInstance instance, XrSystemId systemId)
    : m_xrInstance(instance), systemId_(systemId) {
  loadMetaVirtualKeyboardFunctions();
}

bool MetaVirtualKeyboard::isVirtualKeyboardSupported() {
  XrSystemProperties systemProperties{XR_TYPE_SYSTEM_PROPERTIES};
  XrSystemVirtualKeyboardPropertiesMETA virtualKeyboardProps{
    XR_TYPE_SYSTEM_VIRTUAL_KEYBOARD_PROPERTIES_META};
  systemProperties.next = &virtualKeyboardProps;
  OPENXR_CHECK(
    xrGetSystemProperties(m_xrInstance, systemId_, &systemProperties),
    "Failed to call xrGetSystemProperties");
  return virtualKeyboardProps.supportsVirtualKeyboard == XR_TRUE;
}

void MetaVirtualKeyboard::loadMetaVirtualKeyboardFunctions() {
  loadXrProcOrDie(m_xrInstance, "xrCreateVirtualKeyboardMETA", &xrCreateVirtualKeyboardMETA_);
  loadXrProcOrDie(m_xrInstance, "xrDestroyVirtualKeyboardMETA", &xrDestroyVirtualKeyboardMETA_);
  loadXrProcOrDie(
    m_xrInstance, "xrCreateVirtualKeyboardSpaceMETA", &xrCreateVirtualKeyboardSpaceMETA_);
  loadXrProcOrDie(
    m_xrInstance, "xrSuggestVirtualKeyboardLocationMETA", &xrSuggestVirtualKeyboardLocationMETA_);
  loadXrProcOrDie(m_xrInstance, "xrGetVirtualKeyboardScaleMETA", &xrGetVirtualKeyboardScaleMETA_);
  loadXrProcOrDie(
    m_xrInstance,
    "xrSetVirtualKeyboardModelVisibilityMETA",
    &xrSetVirtualKeyboardModelVisibilityMETA_);
  loadXrProcOrDie(
    m_xrInstance,
    "xrGetVirtualKeyboardModelAnimationStatesMETA",
    &xrGetVirtualKeyboardModelAnimationStatesMETA_);
  loadXrProcOrDie(
    m_xrInstance, "xrGetVirtualKeyboardDirtyTexturesMETA", &xrGetVirtualKeyboardDirtyTexturesMETA_);
  loadXrProcOrDie(
    m_xrInstance, "xrGetVirtualKeyboardTextureDataMETA", &xrGetVirtualKeyboardTextureDataMETA_);
  loadXrProcOrDie(m_xrInstance, "xrSendVirtualKeyboardInputMETA", &xrSendVirtualKeyboardInputMETA_);
  loadXrProcOrDie(
    m_xrInstance, "xrChangeVirtualKeyboardTextContextMETA", &xrChangeVirtualKeyboardTextContext_);
}

}  // namespace c8
