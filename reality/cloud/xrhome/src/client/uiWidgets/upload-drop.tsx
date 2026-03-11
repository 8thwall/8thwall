import * as React from 'react'
import {Button} from 'semantic-ui-react'
import '../static/styles/upload-drop.scss'

export interface IUploadDropProps {
  uploadMessage?: string
  dropMessage?: string  // shown on file hover
  onDrop(file: File): void  // what to do when the user upload
  fileAccept?: string  // e.g. '*/*'
  className?: string
  elementClickInsteadOfButton: boolean  // when true, the entire element is clickable instead of the button
  noButton: boolean  // when true, no button is shown
  children?: React.ReactNode
}

export class UploadDrop extends React.Component<IUploadDropProps> {
  state = {
    hovering: false,
    dropErrorMessage: null,
  }

  inputFileRef: any

  constructor(props) {
    super(props)
    this.inputFileRef = React.createRef()
  }

  onDrop = (e) => {
    e.preventDefault()
    if (e.dataTransfer && e.dataTransfer.files.length === 0 && (!e.target.files || e.target.files.length === 0)) {
      this.setState({
        dropErrorMessage: 'There was no file attached in this drop. You can drop a file from a file browser or ' +
        'click this element to open the file picker',
      })
      return
    }

    const file = (e.dataTransfer && e.dataTransfer.files[0]) || e.target.files[0]
    if (this.props.fileAccept != '*/*') {
      const acceptableExtension = !!(this.props.fileAccept.split(',').find(ext => file.name.toLowerCase().endsWith(ext.trim())))
      if (!acceptableExtension) {
        this.setState({dropErrorMessage: `Please drop only files with extensions ${this.props.fileAccept}`})
        return
      }
    }

    this.props.onDrop(file)
    this.setState({dropErrorMessage: null})
  }

  buttonClicked = () => this.inputFileRef.current && this.inputFileRef.current.click()

  render() {
    return (
      <div
        className={`upload-drop ${this.state.hovering && 'hovering'} ${this.props.className} ${this.props.elementClickInsteadOfButton && 'clickable'}`}
        onDragEnter={(e) => { e.preventDefault(); this.setState({hovering: true}) }}
        onDragLeave={e => this.setState({hovering: false})}
        onDragOver={e => e.preventDefault()}
        onDrop={(e) => { this.onDrop(e); this.setState({hovering: false}) }}
        onClick={() => { if (this.props.elementClickInsteadOfButton) this.buttonClicked() }}
      >
        <div className='drop-target'>
          {!this.props.elementClickInsteadOfButton && !this.props.noButton &&
            <>
              <Button
                primary
                style={{position: 'relative'}}
                onClick={this.buttonClicked}
                content='Upload'
              />
              {this.props.uploadMessage && `or ${this.props.uploadMessage}`}
            </>
          }
          <input style={{display: 'none'}} type='file' accept={this.props.fileAccept} ref={this.inputFileRef} onChange={this.onDrop} value='' />
          <div className='drop-instructions'>{this.props.children}</div>
        </div>
        {this.props.dropMessage && <div className='drop-message'>{this.props.dropMessage}</div>}
        {this.state.dropErrorMessage && <div className='drop-error-message'>{this.state.dropErrorMessage}</div>}
      </div>
    )
  }
}
