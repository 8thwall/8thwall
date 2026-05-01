// Oculus events taken from:
// https://github.com/aframevr/aframe/blob/master/docs/components/oculus-touch-controls.md
const OCULUS_TOUCH_EVENTS = [
  'triggerdown',           // Trigger pressed.
  'triggerup',             // Trigger released.
  'triggertouchstart',     // Trigger touched.
  'triggertouchend',       // Trigger no longer touched.
  'triggerchanged',        // Trigger changed.
  'thumbstickdown',        // Thumbstick pressed.
  'thumbstickup',          // Thumbstick released.
  'thumbsticktouchstart',  // Thumbstick touched.
  'thumbsticktouchend',    // Thumbstick no longer touched.
  'thumbstickchanged',     // Thumbstick changed.
  'thumbstickmoved',       // Thumbstick direction changed.
  'gripdown',              // Grip button pressed.
  'gripup',                // Grip button released.
  'griptouchstart',        // Grip button touched.
  'griptouchend',          // Grip button no longer touched.
  'gripchanged',           // Grip button changed.
  'abuttondown',           // A button pressed.
  'abuttonup',             // A button released.
  'abuttontouchstart',     // A button touched.
  'abuttontouchend',       // A button no longer touched.
  'abuttonchanged',        // A button changed.
  'bbuttondown',           // B button pressed.
  'bbuttonup',             // B button released.
  'bbuttontouchstart',     // B button touched.
  'bbuttontouchend',       // B button no longer touched.
  'bbuttonchanged',        // B button changed.
  'xbuttondown',           // X button pressed.
  'xbuttonup',             // X button released.
  'xbuttontouchstart',     // X button touched.
  'xbuttontouchend',       // X button no longer touched.
  'xbuttonchanged',        // X button changed.
  'ybuttondown',           // Y button pressed.
  'ybuttonup',             // Y button released.
  'ybuttontouchstart',     // Y button touched.
  'ybuttontouchend',       // Y button no longer touched.
  'ybuttonchanged',        // Y button changed.
  'surfacedown',           // Surface button pressed.
  'surfaceup',             // Surface button released.
  'surfacetouchstart',     // Surface button touched.
  'surfacetouchend',       // Surface button no longer touched.
  'surfacechanged',        // Surface button changed.
]

// https://github.com/aframevr/aframe/blob/master/docs/components/magicleap-controls.md
const MAGICLEAP_EVENTS = [
  'menudown',
  'menuup',
  'menuchanged',
]

const CONTROLLER_EVENTS = [
  ...new Set([
    ...OCULUS_TOUCH_EVENTS,
    ...MAGICLEAP_EVENTS,
  ]),
]

export default CONTROLLER_EVENTS
