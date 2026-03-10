// Copyright (c) 2019 8th Wall, Inc.
// Original Author: Nicholas Butko (nb@8thwall.com)
#pragma once

#include "c8/geometry/parameterized-geometry.h"
#include "c8/geometry/pixel-pinhole-camera-model.h"
#include "c8/io/capnp-messages.h"
#include "reality/engine/api/reality.capnp.h"
#include "reality/engine/features/gl-reality-frame.h"
#include "reality/engine/features/gr8-feature-shader.h"
#include "reality/engine/features/gr8gl.h"
#include "reality/engine/imagedetection/detection-image.h"

namespace c8 {

class DetectionImageLoader {
public:
  DetectionImageLoader() : featureDetector_(Gr8Gl::create()) {}

  // Allocates GPU and CPU resources for processing the detection image.
  void initialize(
    Gr8FeatureShader *glShader,
    ImageTargetMetadata::Reader imageTargetMetadataReader,
    c8_PixelPinholeCameraModel k);

  // Set the geometry and fullLabelToCroppedLabelPose for the DetectionImage we are going to load
  // We will also scale the pixel size of this geometry to the size of our extracted features
  // pixel space: level 0 of our pyramid layout. That way if you use the loaded DetectionImage
  // curvy geometry on features extracted, their pixel spaces are the same.
  void setGeometry(int cropWidth, int cropHeight, CurvySpec spec);

  GlTexture imTexture() { return imTexture_.tex(); }

  // Set up all the gpu operations to start computing features.
  void processGpu();

  // Read data off of the gpu into memory where it can be processed by cpu.
  // Call this after processGpu.
  void readDataToCpu();

  // Extract feature points and descriptors from the pyramid.
  // Call this after readDataToCpu.
  DetectionImage extractFeatures();

  String name() { return name_; }
  GlRealityFrame &gl() { return gl_; }
  const GlRealityFrame &gl() const { return gl_; }
  int rotation() const { return rotation_; }

  static CurvySpec buildCurvySpec(ImageTargetMetadata::Reader imageTargetMetadataReader);

  static HMatrix buildFullLabelToCroppedLabelPose(CurvySpec curvySpec);

  const bool rotateFeatures() const { return rotateFeatures_; }

private:
  String name_;
  GlTexture2D imTexture_;
  GlRealityFrame gl_;
  Gr8Gl featureDetector_;
  c8_PixelPinholeCameraModel k_{};
  DetectionImageType detectionImageType_;
  PlanarImageGeometry planarGeom_;
  CurvyImageGeometry curvyGeom_;
  CurvyImageGeometry curvyFullGeom_;
  HMatrix fullLabelToCroppedLabelPose_ = HMatrixGen::i();
  // Whether the image had width > height, and so the pixels must be rotated to be upright.
  int rotation_ = 0;
  // Set to true for images from xrhome which have been rotated.
  // Indicates this is an upright 3:4 image in which the pixels have been rotated 90 degrees
  // clockwise. We want to return the pose of this image upright, i.e. as if we did a 90 degrees
  // counter-clockwise rotation resulting in a 4:3 image. We do this by leaving the image as-is
  // GlRealityFrame and rotating the extracted feature points and intrinsics .
  bool isRotated_ = false;  // metadata of whether the image is rotated when loaded.
  DescriptorType featureType_ = DescriptorType::ORB;
  // rotateFeatures_ is used to handle rotation of features and ROI for P2P.
  bool rotateFeatures_ = false;
};

}  // namespace c8
