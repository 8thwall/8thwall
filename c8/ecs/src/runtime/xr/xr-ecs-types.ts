interface EcsRenderOverride {
  engage(): void
  disengage(): void
  render(dt: number): void
}

export type {
  EcsRenderOverride,
}
