/* eslint-disable local-rules/hardcoded-copy -- Legacy validation messages. */
import * as React from 'react'
import '../static/styles/upload-drop.scss'

export interface IUploadDropProps {
  dropMessage?: string  // shown on file hover
  onDrop(file: File): void  // what to do when the user upload
  fileAccept: string  // e.g. '*/*'
  className?: string
  children?: React.ReactNode
}

export class UploadDrop extends React.Component<IUploadDropProps> {
  state = {
    hovering: false,
    dropErrorMessage: null,
  }

  onDrop = (e) => {
    e.preventDefault()
    const noDroppedFile = e.dataTransfer && e.dataTransfer.files.length === 0
    const noSelectedFile = !e.target.files || e.target.files.length === 0
    if (noDroppedFile && noSelectedFile) {
      this.setState({
        dropErrorMessage: 'There was no file attached in this drop. ' +
          'You can drop a file from a file browser or click this element to open the file picker',
      })
      return
    }

    const file = (e.dataTransfer && e.dataTransfer.files[0]) || e.target.files[0]
    if (this.props.fileAccept !== '*/*') {
      const acceptableExtension = !!this.props.fileAccept.split(',').find(
        ext => file.name.toLowerCase().endsWith(ext.trim())
      )
      if (!acceptableExtension) {
        this.setState({
          dropErrorMessage: `Please drop only files with extensions ${this.props.fileAccept}`,
        })
        return
      }
    }

    this.props.onDrop(file)
    this.setState({dropErrorMessage: null})
  }

  render() {
    return (
      <div
        className={
          `upload-drop clickable ${this.state.hovering && 'hovering'} ${this.props.className}`
        }
        onDragEnter={(e) => {
          e.preventDefault(); this.setState({hovering: true})
        }}
        onDragLeave={() => this.setState({hovering: false})}
        onDragOver={e => e.preventDefault()}
        onDrop={(e) => {
          this.onDrop(e); this.setState({hovering: false})
        }}
      >
        <div className='drop-target'>
          <input
            style={{display: 'none'}}
            type='file'
            accept={this.props.fileAccept}
            onChange={this.onDrop}
            value=''
          />
          <div className='drop-instructions'>{this.props.children}</div>
        </div>
        {this.props.dropMessage && (
          <div className='drop-message'>{this.props.dropMessage}</div>
        )}
        {this.state.dropErrorMessage && (
          <div className='drop-error-message'>{this.state.dropErrorMessage}</div>
        )}
      </div>
    )
  }
}
