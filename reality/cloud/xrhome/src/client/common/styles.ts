type MaybeClass = string | false | undefined | null

const combine = (class1: MaybeClass, class2: MaybeClass, ...rest: MaybeClass[]) => (
  [class1, class2, ...rest].filter(v => v).join(' ')
)

const bool = (v: any, className: string) => (v ? className : '')

export {
  combine,
  bool,
}
