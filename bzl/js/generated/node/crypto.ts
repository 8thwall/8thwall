import * as nodeCrypto from 'crypto'

// Look for the SubtleCrypto interface in the crypto module under .subtle or .webcrypto.subtle.
// If not found, use any.
type SubtleCryptoIfAvailable<T> =
  T extends { subtle: infer P } ? P : T extends {webcrypto: { subtle: infer Q } } ? Q : any

type SubtleCrypto = SubtleCryptoIfAvailable<typeof nodeCrypto>

interface GeneratedCrypto {
  subtle: SubtleCrypto,
}

// Node 15.0+ uses crypto.webcrypto.subtle; Node 17.4+ uses crypto.subtle
// However, @types/node can't resolve either of them, so we need casts here.
const crypto: GeneratedCrypto = {
  subtle: (nodeCrypto as any).subtle
    ? (nodeCrypto as any).subtle as SubtleCrypto
    : (nodeCrypto as any).webcrypto.subtle as SubtleCrypto,
}

export {
  crypto,
}
