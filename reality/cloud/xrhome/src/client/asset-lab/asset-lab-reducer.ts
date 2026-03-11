import type {
  AssetLabState, GetAssetGenerationsAction, GetAssetRequestsAction, RequestsByType,
  UpdateAssetGenerationMetadataAction,
} from './types'
import {
  AL_GET_ASSET_GENERATIONS,
  AL_GET_ASSET_REQUESTS,
  AL_UPDATE_ASSET_GENERATION_METADATA,
  ActionType,
  Handler,
} from './action-types'
import type {AssetGenerationAssetType} from '../common/types/db'

const toTypeString = (
  type: AssetGenerationAssetType
): keyof RequestsByType => {
  switch (type) {
    case 'IMAGE':
      return 'image'
    case 'MESH':
      return 'model'
    // No animation support yet
    default:
      return 'image'
  }
}

const initialState: AssetLabState = {
  assetGenerations: {},
  assetRequests: {},
  assetGenerationsByAccount: {},
  assetRequestsByAccount: {},
  assetGenerationsByAssetRequest: {},
  assetRequestsByTypeByAccount: {},
}

const handlers: Record<ActionType, Handler> = {
  [AL_GET_ASSET_GENERATIONS]: (
    state: AssetLabState, action: GetAssetGenerationsAction
  ): AssetLabState => {
    const assetGenerations = action.assetGenerations.reduce(
      (acc, assetGeneration) => {
        acc[assetGeneration.uuid] = assetGeneration
        return acc
      }, {}
    )
    const newAssetGenerationsByAssetRequest = action.assetGenerations.reduce(
      (acc, assetGeneration) => {
        const {uuid, RequestUuid} = assetGeneration
        if (!acc[RequestUuid]) {
          acc[RequestUuid] = []
        }
        acc[RequestUuid].push(uuid)
        return acc
      }, {}
    )

    const newAssetGenerationsByAccount: Record<string, string[]> = action.assetGenerations.reduce(
      (acc, assetGeneration) => {
        const {AccountUuid, uuid} = assetGeneration
        acc[AccountUuid] = acc[AccountUuid] || [
          ...(state.assetGenerationsByAccount[AccountUuid] || []),
        ]
        acc[AccountUuid].push(uuid)
        return acc
      }, {}
    )
    // Clean up potential duplicates in newAssetGenerationsByAccount
    Object.entries(newAssetGenerationsByAccount).forEach(([accountUuid, genUuids]) => {
      const uniqueGenUuids = new Set(genUuids)
      newAssetGenerationsByAccount[accountUuid] = Array.from(uniqueGenUuids)
    })

    const newAssetRequestsByType = Object.entries(newAssetGenerationsByAssetRequest).reduce(
      (acc, [assetRequestUuid, assetGenUuids]) => {
        // Assuming that the first asset generation has the type of the entire group
        const assetGen = assetGenerations[assetGenUuids[0]]
        acc[toTypeString(assetGen.assetType)].push(assetRequestUuid)
        return acc
      }, {
        image: [...(state.assetRequestsByTypeByAccount[action.accountUuid]?.image || [])],
        model: [...(state.assetRequestsByTypeByAccount[action.accountUuid]?.model || [])],
        animation: [...(state.assetRequestsByTypeByAccount[action.accountUuid]?.animation || [])],
      }
    )
    return {
      ...state,
      assetGenerations: {
        ...state.assetGenerations,
        ...assetGenerations,
      },
      assetGenerationsByAccount: {
        ...state.assetGenerationsByAccount,
        ...newAssetGenerationsByAccount,
      },
      assetGenerationsByAssetRequest: {
        ...state.assetGenerationsByAssetRequest,
        ...newAssetGenerationsByAssetRequest,
      },
      assetRequestsByTypeByAccount: {
        ...state.assetRequestsByTypeByAccount,
        [action.accountUuid]: {
          ...newAssetRequestsByType,
        },
      },
    }
  },

  [AL_GET_ASSET_REQUESTS]: (
    state: AssetLabState, action: GetAssetRequestsAction
  ): AssetLabState => {
    const assetRequests = action.assetRequests.reduce(
      (acc, assetRequest) => {
        acc[assetRequest.uuid] = assetRequest
        return acc
      }, {}
    )
    const newAssetRequests = {
      ...state.assetRequests,
      ...assetRequests,
    }

    const assetRequestsByAccount: Record<string, string[]> = action.assetRequests.reduce(
      (acc, assetRequest) => {
        const {uuid, AccountUuid} = assetRequest
        acc[AccountUuid] = acc[AccountUuid] || []
        acc[AccountUuid].push(uuid)
        return acc
      }, {}
    )

    // Merge with existing state for AccountUuid I have seen
    const updatedAssetRequestsByAccount = {}
    Object.entries(assetRequestsByAccount).forEach(([accountUuid, requestUuids]) => {
      const newRequestUuids = new Set([
        ...(state.assetRequestsByAccount[accountUuid] || []),
        ...requestUuids,
      ])
      updatedAssetRequestsByAccount[accountUuid] = Array.from(newRequestUuids).sort(
        (a, b) => newAssetRequests[b].createdAt.localeCompare(newAssetRequests[a].createdAt)
      )
    })

    const newAssetRequestsByType = action.assetRequests.reduce((acc, assetRequest) => {
      if (assetRequest.AssetGenerations && assetRequest.AssetGenerations.length > 0) {
        // Assuming that the first asset generation has the type of the entire group
        acc[toTypeString(assetRequest.AssetGenerations[0].assetType)].push(assetRequest.uuid)
      }
      return acc
    }, {
      image: [...(state.assetRequestsByTypeByAccount[action.accountUuid]?.image || [])],
      model: [...(state.assetRequestsByTypeByAccount[action.accountUuid]?.model || [])],
      animation: [...(state.assetRequestsByTypeByAccount[action.accountUuid]?.animation || [])],
    })

    const newAssetGenerations = action.assetRequests.reduce((acc, assetRequest) => {
      const {AssetGenerations} = assetRequest
      if (AssetGenerations && AssetGenerations.length > 0) {
        AssetGenerations.forEach((assetGeneration) => {
          acc[assetGeneration.uuid] = assetGeneration
        })
      }
      return acc
    }, {})

    const newAssetGenerationsByAssetRequest = action.assetRequests.reduce((acc, assetRequest) => {
      const {uuid, AssetGenerations} = assetRequest
      if (AssetGenerations && AssetGenerations.length > 0) {
        acc[uuid] = AssetGenerations.map(gen => gen.uuid)
      }
      return acc
    }, {})

    const newAssetGenerationsByAccount = action.assetRequests.reduce((acc, assetRequest) => {
      const {AccountUuid, AssetGenerations} = assetRequest
      if (AssetGenerations && AssetGenerations.length > 0) {
        acc[AccountUuid] = acc[AccountUuid] || [
          ...(state.assetGenerationsByAccount[AccountUuid] || []),
        ]
        AssetGenerations.forEach((assetGeneration) => {
          acc[AccountUuid].push(assetGeneration.uuid)
        })
      }
      return acc
    }, {} as Record<string, string[]>)

    // Clean up potential duplicates in newAssetGenerationsByAccount
    Object.entries(newAssetGenerationsByAccount).forEach(([accountUuid, genUuids]) => {
      const uniqueGenUuids = new Set(genUuids)
      newAssetGenerationsByAccount[accountUuid] = Array.from(uniqueGenUuids)
    })

    // Now we can merge with the state
    return {
      ...state,
      assetRequests: newAssetRequests,
      assetRequestsByAccount: {
        ...state.assetRequestsByAccount,
        ...updatedAssetRequestsByAccount,
      },
      assetRequestsByTypeByAccount: {
        ...state.assetRequestsByTypeByAccount,
        [action.accountUuid]: {
          ...newAssetRequestsByType,
        },
      },
      assetGenerations: {
        ...state.assetGenerations,
        ...newAssetGenerations,
      },
      assetGenerationsByAssetRequest: {
        ...state.assetGenerationsByAssetRequest,
        ...newAssetGenerationsByAssetRequest,
      },
      assetGenerationsByAccount: {
        ...state.assetGenerationsByAccount,
        ...newAssetGenerationsByAccount,
      },
    }
  },

  [AL_UPDATE_ASSET_GENERATION_METADATA]: (
    state: AssetLabState, action: UpdateAssetGenerationMetadataAction
  ) => ({
    ...state,
    assetGenerations: {
      ...state.assetGenerations,
      [action.assetGenerationUuid]: {
        ...state.assetGenerations[action.assetGenerationUuid],
        metadata: {
          ...state.assetGenerations[action.assetGenerationUuid].metadata,
          ...action.updatedMetadata,
        },
      },
    },
  }),
}

type Action = GetAssetGenerationsAction
  | GetAssetRequestsAction
  | UpdateAssetGenerationMetadataAction

const AssetLabReducer = (state = {...initialState}, action: Action) => (
  handlers[action.type]?.(state, action) || state
)

export default AssetLabReducer
