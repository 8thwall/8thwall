// Copyright (c) 2020 8th Wall, Inc.
// Original Author: Nathan Waters (nathan@8thwall.com)

// Utility methods for determining the visibility of the document across various platforms

// Gets the browser specific terms for when the page is hidden or becomes visible again
const getBrowserVisibilityKeywords = () => {
  let hidden = 'hidden'
  let visibilityChange = 'visibilitychange'
  if (typeof document.hidden !== 'undefined') {
    // continue using our defaults
  } else if (typeof document.mozHidden !== 'undefined') {
    hidden = 'mozHidden'
    visibilityChange = 'mozvisibilitychange'
  } else if (typeof document.msHidden !== 'undefined') {
    hidden = 'msHidden'
    visibilityChange = 'msvisibilitychange'
  } else if (typeof document.webkitHidden !== 'undefined') {
    hidden = 'webkitHidden'
    visibilityChange = 'webkitvisibilitychange'
  } else {
    // eslint-disable-next-line no-console
    console.warn('[XR] Page Visibility API not supported.')
  }
  return {hidden, visibilityChange}
}

const isDocumentVisible = () => {
  const {hidden} = getBrowserVisibilityKeywords()
  return !document[hidden]
}

const getVisibilityEventName = () => {
  const {visibilityChange} = getBrowserVisibilityKeywords()
  return visibilityChange
}

export {
  isDocumentVisible,
  getVisibilityEventName,
}
