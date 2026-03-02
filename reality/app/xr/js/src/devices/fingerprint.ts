// We once used fingerprintjs2 to generate a stable fingerprint for the device, but switched to
// a random fingerprint in the same format: 32 digits of hex.
const FINGERPRINT_LENGTH = 32
const FINGERPRINT_BASE = 16
let fingerprint: string | null = null
const getRandomFingerprint = (): string => {
  if (!fingerprint) {
    const hexDigits = Array.from({length: FINGERPRINT_LENGTH},
      () => Math.floor(Math.random() * FINGERPRINT_BASE).toString(FINGERPRINT_BASE))

    fingerprint = hexDigits.join('')
  }
  return fingerprint
}

type OnFingerprintAvailableCb = (fingerprint: string, unused: string[]) => void
const getFingerprint = (onFingerprintAvailable: OnFingerprintAvailableCb) => {
  // The empty array is a holdover from fingerprintjs2 which contained key/value pairs of
  // detected properties.
  onFingerprintAvailable(getRandomFingerprint(), [])
}

export {
  getFingerprint,
  OnFingerprintAvailableCb,
}
