// Copyright (c) 2020 8th Wall, Inc.
// Original Author: Nicholas Butko (nb@8thwall.com)

#pragma once

#include "c8/string.h"
#include "c8/vector.h"
#include "tensorflow/lite/schema/schema_generated.h"

namespace c8 {

struct TfliteGraphStats {
  // Graph operator index that produced a tensor, or -1 if a tensor is primary data or an input.
  Vector<int> tensorProducedByOperator;

  // The operators that have this tensor as input.
  Vector<Vector<int>> tensorProducesOperators;

  // The depth at which the tensor was produced in the operator DAG.
  Vector<int> tensorDepths;

  // The tensors that are produced at a given depth in the operator DAG.
  Vector<Vector<int>> tensorsAtLevel;
};

String tensorStr(int idx, const tflite::SubGraph *g, int indent);
String tensorVecStr(const flatbuffers::Vector<int> *vec, const tflite::SubGraph *g, int level);

String opcodeStr(const tflite::OperatorCode *opcode, int level);
String opcodeVecStr(
  const flatbuffers::Vector<flatbuffers::Offset<tflite::OperatorCode>> *vec, int level);

String metadataStr(const tflite::Metadata *metadata, const tflite::Model *model, int level);
String metadataVecStr(
  const flatbuffers::Vector<flatbuffers::Offset<tflite::Metadata>> *vec,
  const tflite::Model *model,
  int level);

// We need to pass in the parent for type unpacking.
String builtinOptionsStr(const tflite::Operator *op, int level);

String operatorStr(
  const tflite::Operator *op, const tflite::SubGraph *g, const tflite::Model *m, int level);
String operatorVecStr(
  const flatbuffers::Vector<flatbuffers::Offset<tflite::Operator>> *vec,
  const tflite::SubGraph *g,
  const tflite::Model *m,
  int level);

String subgraphStr(const tflite::SubGraph *g, const tflite::Model *m, int level);
String subgraphVecStr(
  const flatbuffers::Vector<flatbuffers::Offset<tflite::SubGraph>> *vec,
  const tflite::Model *m,
  int level);

String modelStr(const tflite::Model *m, int level);

// Analyze the tensor DAG and collect some stats.
TfliteGraphStats getGraphStats(const tflite::SubGraph* g);

// Convert a tflite graph into GraphViz dot format, so that it can be rendered to pdf.
String getGraphVizString(const tflite::SubGraph *g, const tflite::Model *m);

}  // namespace c8
