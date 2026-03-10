// @inliner-off

/* eslint-disable no-var */
declare var XR8: any
declare var _XR8: any
declare var _XR8Chunks: any
declare var THREE: any
// User can force metaversal mode
declare var _XR8MetaversalMode: 'immersive-ar' | 'immersive-vr' | undefined
// When we load THREE r160+, using importmap, we assign GLTFLoader onto window
// eslint-disable-next-line local-rules/acronym-capitalization
declare var GLTFLoader: any
declare var AFRAME: any
declare var omni8: any

// @dep(//reality/app/xr/js:buildif)
declare var BuildIf: import('@repo/reality/app/xr/js/buildif').BuildIfReplacements
