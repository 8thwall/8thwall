import React from 'react'
import type {StudioComponentError} from '@ecs/shared/studio-component'
import {createUseStyles} from 'react-jss'

import {
  deriveLocationFromKey, extractFilePath, type EditorFileLocation,
} from '../editor/editor-file-location'
import {useStudioComponentsContext} from './studio-components-context'
import {StaticBanner} from '../ui/components/banner'
import {useFileActionsContext} from '../editor/files/file-actions-context'
import {BoldButton} from '../ui/components/bold-button'

// TODO: Finish translating
/* eslint-disable local-rules/hardcoded-copy */

const useStyles = createUseStyles({

  errorText: {
    textWrap: 'nowrap',
  },
})

interface IErrorMessageLink {
  firstErrorLocation: EditorFileLocation
  firstError: StudioComponentError
}

const ErrorMessageLink: React.FC<IErrorMessageLink> = ({firstErrorLocation, firstError}) => {
  const classes = useStyles()

  const actionsContext = useFileActionsContext()

  if (!firstError.location) {
    return null
  }

  return (
    <BoldButton onClick={() => actionsContext.onSelect(firstErrorLocation)}>
      <span className={classes.errorText}>
        {`[Ln ${firstError.location.startColumn}, Col ${firstError.location.startLine}]`}
      </span>
    </BoldButton>
  )
}

const ErrorMessage: React.FC = () => {
  const {errors} = useStudioComponentsContext()

  let firstErrorLocation: EditorFileLocation
  let firstError: StudioComponentError
  const numErrors = errors && Object.keys(errors).reduce((acc, key) => {
    if (errors[key].length && !firstError) {
      firstErrorLocation = deriveLocationFromKey(key)
      ;[firstError] = errors[key]
    }
    return acc + errors[key].length
  }, 0)

  if (!numErrors) {
    return null
  }

  return (
    <StaticBanner type='danger'>
      {numErrors > 1 &&
        <>
          A total of ${numErrors} errors
          <br />
        </>
      }
      {`(${extractFilePath(firstErrorLocation)}): `}
      <br />
      {firstError.message}
      {numErrors > 1 && <><br />{`${numErrors - 1} more...`}</>}
      <ErrorMessageLink
        firstErrorLocation={firstErrorLocation}
        firstError={firstError}
      />
    </StaticBanner>
  )
}

export {ErrorMessage}
