/*

NOTE(christoph): Currently we are on edition: 0, so the array ist empty, but when we
publish an edition 1, we should include all features from edition in an array like:

export const EDITIONS: string[][] = [
  ['jolt', 'another-feature'], // Edition 1
]

This will remove the explicitly listed features from the runtime metadata, and bump the
edition number to 1.
*/

export const EDITIONS: string[][] = [

]
