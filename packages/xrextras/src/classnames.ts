type ClassName = string | false | null | undefined

const combine = (...classNames: ClassName[]) => classNames.filter(Boolean).join(' ')

export {combine}
