import React from 'react'

// NOTE(christoph): The useGLTF hook can throw errors if it fails to load. We don't want this to
// crash the whole page so we're going to catch these errors. The Suspense prevents the whole
// page from becoming a loading screen while the GLTF is loading.
class GltfLoadBoundary extends React.Component<{children: React.ReactNode}> {
  state = {hasError: false}

  static getDerivedStateFromError() {
    return {hasError: true}
  }

  render() {
    if (this.state.hasError) {
      return null
    }

    return (
      <React.Suspense fallback={null}>
        {this.props.children}
      </React.Suspense>
    )
  }
}

export {
  GltfLoadBoundary,
}
