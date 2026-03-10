// Copyright (c) 2023 Niantic, Inc.
// Original Author: Lynn Dang (lynndang@nianticlabs.com)

#pragma once

#include "c8/pixels/pixel-buffer.h"
#include "c8/pixels/pixels.h"
#include "c8/vector.h"
#include "tflite-interpreter.h"

namespace c8 {

// Wrapper class for a tflite multiclassifier.
class TfLiteMultiClassifier {
public:
  // Construct from a tflite model.
  TfLiteMultiClassifier(
    const uint8_t *modelData,
    int modelSize,
    size_t modelCacheSize,
    int inputHeight,
    int inputWidth,
    int numClasses);
  TfLiteMultiClassifier(
    const Vector<uint8_t> &modelData,
    size_t modelCacheSize,
    int inputHeight,
    int inputWidth,
    int numClasses);

  // Default move constructors.
  TfLiteMultiClassifier(TfLiteMultiClassifier &&) = default;
  TfLiteMultiClassifier &operator=(TfLiteMultiClassifier &&) = default;

  // Disallow copying.
  TfLiteMultiClassifier(const TfLiteMultiClassifier &) = delete;
  TfLiteMultiClassifier &operator=(const TfLiteMultiClassifier &) = delete;

  int getInputWidth() const { return inputWidth_; }
  int getInputHeight() const { return inputHeight_; }
  int getNumberOfClasses() const { return numClasses_; }

  void processOutput(ConstRGBA8888PlanePixels inputImage);
  // Given an image, return a vector of float pixels with the class scores from the model.
  // The outputClassIds is the vector of class Ids for segmentationResults.
  // If outputClassIds is empty, fill the segmentationResults with results from all classes.
  void generateOutput(
    ConstRGBA8888PlanePixels inputImage,
    Vector<FloatPixels> &multiclassResults,
    const Vector<int> outputClassIds);

  // Given an image, return float pixels for the specified class from the model.
  ConstOneChannelFloatPixels obtainSingleOutput(ConstRGBA8888PlanePixels inputImage, int classId);

  // Extracts pixels from an output tensor which is packed with each class in a row, i.e.
  // [class1Pixel1, class2Pixel1, ..., classNPixel1, class1Pixel2, class2Pixel2, etc.]
  //
  // If you have classes [Hello, Foo, Bar] and want to get Bar from the tensor into dest:
  //    const float *multiclassOutput = interpreter_->typed_output_tensor<float>(0);
  //    const float *src = multiclassOutput + 2; // Hello would be 0, Foo would be 1, Bar is 2.
  //    outTensorToClassImage(src, dest, 3);   // There are 3 classes in this example.
  //
  // @param src the tensor.
  // @param dest the destination to write the pixels to.
  // @param numClasses the number of classes in the tensor.
  void outTensorToClassImage(const float *src, FloatPixels dest);

private:
  TFLiteInterpreter interpreter_;                         // Interpreter for segmentation tflite.
  Vector<OneChannelFloatPixelBuffer> multiclassBuffers_;  // Image buffers for the classes.
  std::unique_ptr<OneChannelFloatPixelBuffer> singleclassBuffer_;

  int inputWidth_;
  int inputHeight_;
  int numClasses_;
  void initInputOutputDims();
};

}  // namespace c8
