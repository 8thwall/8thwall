import {createHash, pseudoRandomBytes} from 'crypto'

type SeedValue = string | Buffer

type IdMapper = {
  fromId: (id: string) => string
}

// NOTE(christoph): Leaving room for extensibility here
// for if we need sequentially stable IDs
type IdSeed = IdMapper

const uuidFromBytes = (bytes: Uint8Array): string => {
  if (bytes.length !== 32) {
    throw new Error('Input byte array must be 32 bytes long')
  }

  // UUIDv4 format compliance
  // eslint-disable-next-line no-bitwise
  bytes[6] = (bytes[6] & 0x0f) | 0x40  // version 4
  // eslint-disable-next-line no-bitwise
  bytes[8] = (bytes[8] & 0x3f) | 0x80  // variant 10

  const hex = Array.from(bytes).map(b => b.toString(16).padStart(2, '0'))

  return [
    hex.slice(0, 4).join(''),
    hex.slice(4, 6).join(''),
    hex.slice(6, 8).join(''),
    hex.slice(8, 10).join(''),
    hex.slice(10, 16).join(''),
  ].join('-')
}

const makeHashedUuid = (input: Array<string | Buffer>): string => {
  if (input.length === 0) {
    throw new Error('Input to uuid generation cannot be empty')
  }

  const hash = createHash('sha256')
  input.forEach((item) => {
    hash.update(item)
  })

  const data = hash.digest()
  return uuidFromBytes(data)
}

const fromId = (seed: string, id: string): string => makeHashedUuid([seed, id])

const createFixedSeed = (seed: SeedValue): IdSeed => ({
  fromId: fromId.bind(null, seed),
})

const createIdSeed = () => createFixedSeed(pseudoRandomBytes(256))

export {
  createIdSeed,
  createFixedSeed,
}

export type {
  IdMapper,
  IdSeed,
}
