import React from 'react'

interface IGetSnapshotBeforeUpdate {
  getSnapshotBeforeUpdateCb?: (prevProps, prevState) => any
  componentDidUpdateCb?: (prevProps, prevState, snapshot) => void
}

// This class is created to use getSnapshotBeforeUpdate until React Hook include something
// compatible
class GetSnapshotBeforeUpdate extends React.Component<IGetSnapshotBeforeUpdate> {
  getSnapshotBeforeUpdate = (prevProps, prevState) => (
    this.props.getSnapshotBeforeUpdateCb?.(prevProps, prevState)
  )

  // Prevents warning.
  componentDidUpdate = (prevProps, prevState, snapshot) => {
    this.props.componentDidUpdateCb?.(prevProps, prevState, snapshot)
  };

  render = () => null;
}

export {GetSnapshotBeforeUpdate}
