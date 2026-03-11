// @visibility(//visibility:public)

/* eslint-disable arrow-parens */
// https://docs.aws.amazon.com/amazondynamodb/latest/APIReference/API_AttributeValue.html

type GenericAttribute<T extends string, A extends any> = Omit<NeverAttributes, T> & Record<T, A>
type NeverAttributes = {
  S?: never, N?: never, BOOL?: never
  B?: never, SS?: never, NS?: never, BS?: never, M?: never, L?: never
  NULL?: never, $unknown?: never
}

type StringAttribute<T extends string> = GenericAttribute<'S', T>
type NumberAttribute = GenericAttribute<'N', string>
type BooleanAttribute<T extends boolean> = GenericAttribute<'BOOL', T>
type StringSetAttribute<T extends Array<string>> = GenericAttribute<'SS', T>
type NumberSetAttribute = GenericAttribute<'NS', string[]>
type ListAttribute<T extends Array<Attribute>> = GenericAttribute<'L', T>
type MapAttribute<T extends {[k: string]: Attribute}> = GenericAttribute<'M', T>

type Attribute = StringAttribute<string>
  | NumberAttribute
  | BooleanAttribute<boolean>
  | StringSetAttribute<Array<string>>
  | NumberSetAttribute
  | ListAttribute<Array<Attribute>>
  | MapAttribute<{}>

type AttributeForRaw<T> = (
  T extends string
    ? StringAttribute<T>
    : (
      T extends number
        ? NumberAttribute
        : (
          T extends boolean
            ? BooleanAttribute<T>
            : (
              T extends Array<string>
                ? StringSetAttribute<T>
                : (
                  T extends Array<number>
                    ? NumberSetAttribute
                    : (
                      T extends Array<infer E>
                        ? ListAttribute<Array<AttributeForRaw<E>>>
                        : (
                          T extends {[key: string]: any}
                            ? MapAttribute<{[K in keyof T]: AttributeForRaw<T[K]>}>
                            : never
                        )
                    )
                )
            )
        )
    )
)

type RawForAttribute<T> = (
  T extends StringAttribute<string>
    ? T['S']
    : (
      T extends NumberAttribute
        ? number
        : (
          T extends BooleanAttribute<boolean>
            ? T['BOOL']
            : (
              T extends StringSetAttribute<infer R>
                ? R
                : (
                  T extends NumberSetAttribute
                    ? number[]
                    : (
                      T extends ListAttribute<infer R>
                        ? R extends Array<infer El> ? Array<RawForAttribute<El>> : never
                        : (
                          T extends MapAttribute<infer R>
                            ? {[K in keyof R]: RawForAttribute<R[K]>}
                            : never
                        )
                    )
                )
            )
        )
    )
)

type BaseType = string | number | boolean | BaseItem | Array<BaseType>
type BaseItem = {[k: string]: BaseType}

type AttributeItem = Record<string, Attribute>

type AttributesForRaw<T extends BaseItem> = {[k in keyof T]: AttributeForRaw<T[k]>}
type RawForAttributes<T extends AttributeItem> = {[k in keyof T]: RawForAttribute<T[k]>}

const toAttribute = <T extends BaseType>(value: T): AttributeForRaw<T> => {
  switch (typeof value) {
    case 'string':
      return {S: value} as AttributeForRaw<T>
    case 'number':
      return {N: value.toString()} as AttributeForRaw<T>
    case 'boolean':
      return {BOOL: value} as AttributeForRaw<T>
    case 'object': {
      if (Array.isArray(value)) {
        if (value.every((v) => typeof v === 'string')) {
          return {SS: value} as AttributeForRaw<T>
        }
        if (value.every((v) => typeof v === 'number')) {
          return {NS: value.map(v => v.toString())} as AttributeForRaw<T>
        }
        return {L: value.map(v => toAttribute(v))} as AttributeForRaw<T>
      }
      // eslint-disable-next-line @typescript-eslint/no-use-before-define
      return {M: toAttributes(<BaseItem>value)} as AttributeForRaw<T>
    }
    default:
      throw new Error(`Unable to transform value of type: ${typeof value} to attribute: ${value}`)
  }
}

const fromAttribute = <T extends Attribute>(attribute: T): RawForAttribute<T> => {
  const typeKey = Object.keys(attribute)[0]
  switch (typeKey) {
    case 'S':
      return attribute.S as RawForAttribute<T>
    case 'N':
      return Number.parseFloat(attribute.N) as RawForAttribute<T>
    case 'BOOL':
      return !!attribute.BOOL as RawForAttribute<T>
    case 'SS':
      return attribute.SS as RawForAttribute<T>
    case 'NS':
      return attribute.NS.map((v) => Number.parseFloat(v)) as any as RawForAttribute<T>
    case 'L':
      return attribute.L.map((v) => fromAttribute(v)) as RawForAttribute<T>
    case 'M':
      // eslint-disable-next-line @typescript-eslint/no-use-before-define
      return fromAttributes(attribute.M) as RawForAttribute<T>
    default:
      throw new Error(`Unable to parse attribute with key ${typeKey}`)
  }
}

const fromAttributes = <T extends AttributeItem>(item: T): RawForAttributes<T> => (
  Object.fromEntries(
    Object.entries(item)
      .map(([key, value]) => ([key, fromAttribute(value)]))
  ) as RawForAttributes<T>
)

const toAttributes = <T extends BaseItem>(item: T): AttributesForRaw<T> => (
  Object.fromEntries(
    (Object.entries(item) as [string, BaseType][])
      .filter(([, value]) => value !== undefined)
      .map(([key, value]) => ([key, toAttribute(value)]))
  ) as AttributesForRaw<T>
)

export {
  AttributesForRaw,
  RawForAttributes,
  toAttribute,
  fromAttribute,
  fromAttributes,
  toAttributes,
}
