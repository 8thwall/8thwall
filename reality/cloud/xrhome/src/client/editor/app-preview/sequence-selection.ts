import type {SimulatorState} from '../editor-reducer'
import type {Sequence, VariationConfig, SequenceMetadata} from './app-preview-utils'

type SequenceSelection = {
  sequence: Sequence
  variationName: string
  variation: VariationConfig
}

// NOTE(christoph): We need to turn a "sequence" into a "sequence selection" by resolving the
//   active variation, defaulting to the first in the list.
const resolveVariation = (sequence: Sequence, variationName?: string): SequenceSelection => {
  if (!sequence) {
    return null
  }

  const variation = variationName && sequence.variations[variationName]

  if (!variation) {
    const defaultVariation = Object.keys(sequence.variations)[0]
    return {
      sequence,
      variationName: defaultVariation,
      variation: sequence.variations[defaultVariation],
    }
  }

  return {
    sequence,
    variationName,
    variation,
  }
}

const resolveSequenceByName = (
  sequenceMetadata: SequenceMetadata,
  name: string,
  variationName: string
) => (
  resolveVariation(sequenceMetadata.sequences.find(item => item.name === name), variationName)
)

const resolveSequenceByTag = (
  sequenceMetadata: SequenceMetadata, tag: string
) => (
  resolveVariation(sequenceMetadata.sequences.find(item => item.tag === tag))
)

const resolveDefaultSequence = (
  sequenceMetadata: SequenceMetadata, defaultSimulatorSequence: string
) => {
  if (!defaultSimulatorSequence) {
    return null
  }

  const [tag, sequence, variation] = defaultSimulatorSequence.split('.')

  return (
    resolveSequenceByName(sequenceMetadata, sequence, variation) ||
    resolveSequenceByTag(sequenceMetadata, tag)
  )
}

const resolveFallbackSequence = (sequenceMetadata: SequenceMetadata) => (
  resolveVariation(sequenceMetadata.sequences[0])
)

const resolveSequenceSelection = (
  sequenceMetadata: SequenceMetadata,
  simulatorState: SimulatorState,
  defaultSimulatorSequence?: string
): SequenceSelection => {
  if (!sequenceMetadata) {
    return null
  }

  return (
    resolveSequenceByName(sequenceMetadata, simulatorState.sequence, simulatorState.variation) ||
    resolveDefaultSequence(sequenceMetadata, defaultSimulatorSequence) ||
    resolveFallbackSequence(sequenceMetadata)
  )
}

export {
  resolveSequenceSelection,
}
