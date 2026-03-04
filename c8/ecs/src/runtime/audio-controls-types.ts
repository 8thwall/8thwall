type AudioControls = {
  mute: () => void
  unmute: () => void
  pause: () => void
  play: () => void
  setVolume: (newVolume: number) => void
}

export type {
  AudioControls,
}
