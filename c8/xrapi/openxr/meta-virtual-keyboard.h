#pragma once

#include "c8/xrapi/openxr/openxr.h"

namespace c8 {

class MetaVirtualKeyboard {
public:
  MetaVirtualKeyboard(XrInstance instance, XrSystemId);
  bool isVirtualKeyboardSupported();

private:
  void loadMetaVirtualKeyboardFunctions();

  XrInstance m_xrInstance;
  XrSystemId systemId_;

  PFN_xrCreateVirtualKeyboardMETA xrCreateVirtualKeyboardMETA_ = nullptr;
  PFN_xrDestroyVirtualKeyboardMETA xrDestroyVirtualKeyboardMETA_ = nullptr;
  PFN_xrCreateVirtualKeyboardSpaceMETA xrCreateVirtualKeyboardSpaceMETA_ = nullptr;
  PFN_xrSuggestVirtualKeyboardLocationMETA xrSuggestVirtualKeyboardLocationMETA_ = nullptr;
  PFN_xrGetVirtualKeyboardScaleMETA xrGetVirtualKeyboardScaleMETA_ = nullptr;
  PFN_xrSetVirtualKeyboardModelVisibilityMETA xrSetVirtualKeyboardModelVisibilityMETA_ = nullptr;
  PFN_xrGetVirtualKeyboardModelAnimationStatesMETA xrGetVirtualKeyboardModelAnimationStatesMETA_ =
    nullptr;
  PFN_xrGetVirtualKeyboardDirtyTexturesMETA xrGetVirtualKeyboardDirtyTexturesMETA_ = nullptr;
  PFN_xrGetVirtualKeyboardTextureDataMETA xrGetVirtualKeyboardTextureDataMETA_ = nullptr;
  PFN_xrSendVirtualKeyboardInputMETA xrSendVirtualKeyboardInputMETA_ = nullptr;
  PFN_xrChangeVirtualKeyboardTextContextMETA xrChangeVirtualKeyboardTextContext_ = nullptr;
};

}  // namespace c8