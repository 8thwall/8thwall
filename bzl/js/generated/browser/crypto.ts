interface GeneratedCrypto {
  subtle: SubtleCrypto,
}

const globalThis = window || self
const crypto : GeneratedCrypto = {subtle: globalThis.crypto.subtle}

export {
  crypto,
}
