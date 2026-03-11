type Entry = { key: string, value: unknown }
type RootEntry<T> = { key: '', value: T }

type DotEntries<T> = T extends unknown[]
  ? RootEntry<T>
  : T extends object
    ? {
      [U in keyof T]-?: U extends string
        ?
        DotEntries<T[U]> extends infer V
          ? V extends Entry
            ? V['key'] extends ''
              ? {key: U, value: V['value']}
              : {key: `${U}.${V['key']}`, value: V['value']}
                | {key: U, value: T[U]}
            : never
          : never
        : never
    }[keyof T]
    : RootEntry<T>

type DeepFlatten<T> = {
  [E in DotEntries<T> as E['key']]: E['value']
}

export {
  DeepFlatten,
}
