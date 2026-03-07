interface IModel {
  setInternalConfig(config: { workerUrl: string }): void
  ThreejsModelManager: {
    create(config: { camera: any, renderer: any, config: any }): any
  }
}

export type {
  IModel,
}
