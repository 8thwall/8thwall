const generateIframePermissions = (devUrl: string) => {
  if (!devUrl) {
    return ''
  }

  const {origin} = new URL(devUrl)

  return `\
autoplay ${origin};\
clipboard-write ${origin};\
fullscreen ${origin};\
picture-in-picture ${origin};\
camera ${origin};\
microphone ${origin};\
gyroscope ${origin};\
accelerometer ${origin};\
geolocation ${origin};\
xr-spatial-tracking ${origin}\
`
}

export {
  generateIframePermissions,
}
