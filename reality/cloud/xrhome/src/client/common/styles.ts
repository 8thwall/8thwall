const combine = (class1: string, class2: string, ...rest: string[]) => (
  [class1, class2, ...rest].filter(v => v).join(' ')
)

const bool = (v: any, className: string) => (v ? className : '')

export {
  combine,
  bool,
}
