// Copyright (c) 2020 8th Wall, Inc.
// Original Author: Nicholas Butko (nb@8thwall.com)

#include "bzl/inliner/rules2.h"

cc_library {
  hdrs = {
    "tflite-debug.h",
  };
  deps = {
    "//c8:c8-log",
    "//c8:string",
    "//c8:vector",
    "//c8/string:format",
    "//c8/string:join",
    "@org_tensorflow//tensorflow/lite/schema:schema_fbs",
  };
}
cc_end(0xafc568f2);

#include <iostream>
#include <sstream>

#include "c8/c8-log.h"
#include "c8/string/format.h"
#include "c8/string/join.h"
#include "reality/engine/deepnets/tflite-debug.h"
#include "tensorflow/lite/schema/schema_generated.h"

namespace c8 {

namespace {
String indent(int n) {
  std::stringstream ss;
  for (int i = 0; i < n; ++i) {
    ss << "  ";
  }
  return ss.str();
}

int depthOfTensor(
  int idx,
  Vector<int> &tensorDepths,
  const tflite::SubGraph *g,
  const Vector<int> &tensorProducedByOperator) {
  // If we already computed this value, return it.
  if (tensorDepths[idx] != -1) {
    return tensorDepths[idx];
  }

  auto tensor = (*g->tensors())[idx];
  if (tensorProducedByOperator[idx] == -1) {
    if (idx != 0 && tensor->buffer() == 0) {
      C8Log("!!! tensor %d was not produced but has no data", idx);
    }
    tensorDepths[idx] = 0;
    return 0;
  }

  if (tensor->buffer() != 0) {
    C8Log("!!!! tensor %d has data but was also produced.", idx);
  }
  auto op = (*g->operators())[tensorProducedByOperator[idx]];
  int maxDepth = 0;
  for (int i = 0; i < op->inputs()->size(); ++i) {
    int depth = depthOfTensor((*op->inputs())[i], tensorDepths, g, tensorProducedByOperator);
    if (depth > maxDepth) {
      maxDepth = depth;
    }
  }
  tensorDepths[idx] = maxDepth + 1;
  return maxDepth + 1;
}

String builtinOptionsContents(const tflite::Operator *op, int level) {
  std::stringstream ss;
  auto tab = indent(level);

  if (op->builtin_options_type() == tflite::BuiltinOptions::BuiltinOptions_ConcatenationOptions) {
    auto o = op->builtin_options_as_ConcatenationOptions();
    ss << tab << format("axis: %d,", o->axis()) << std::endl;
    ss << tab
       << format(
            "fused_activation_function: %s,",
            tflite::EnumNamesActivationFunctionType()[o->fused_activation_function()])
       << std::endl;
  } else if (op->builtin_options_type() == tflite::BuiltinOptions::BuiltinOptions_ReshapeOptions) {
    auto o = op->builtin_options_as_ReshapeOptions();
    ss << tab
       << format(
            "new_shape: [%s],",
            strJoin(o->new_shape()->begin(), o->new_shape()->end(), "][").c_str())
       << std::endl;
  } else if (op->builtin_options_type() == tflite::BuiltinOptions::BuiltinOptions_Conv2DOptions) {
    auto o = op->builtin_options_as_Conv2DOptions();
    ss << tab
       << format(
            "fused_activation_function: %s,",
            tflite::EnumNamesActivationFunctionType()[o->fused_activation_function()])
       << std::endl;
    ss << tab << format("padding: %s,", tflite::EnumNamesPadding()[o->padding()]) << std::endl;
    ss << tab << format("stride_w: %d,", o->stride_w()) << std::endl;
    ss << tab << format("stride_h: %d,", o->stride_h()) << std::endl;
    ss << tab << format("dilation_w_factor: %d,", o->dilation_w_factor()) << std::endl;
    ss << tab << format("dilation_h_factor: %d,", o->dilation_h_factor()) << std::endl;
  } else if (op->builtin_options_type() == tflite::BuiltinOptions::BuiltinOptions_Pool2DOptions) {
    auto o = op->builtin_options_as_Pool2DOptions();
    ss << tab
       << format(
            "fused_activation_function: %s,",
            tflite::EnumNamesActivationFunctionType()[o->fused_activation_function()])
       << std::endl;
    ss << tab << format("padding: %s,", tflite::EnumNamesPadding()[o->padding()]) << std::endl;
    ss << tab << format("stride_w: %d,", o->stride_w()) << std::endl;
    ss << tab << format("stride_h: %d,", o->stride_h()) << std::endl;
    ss << tab << format("filter_width: %d,", o->filter_width()) << std::endl;
    ss << tab << format("filter_height: %d,", o->filter_height()) << std::endl;
  } else if (
    op->builtin_options_type() == tflite::BuiltinOptions::BuiltinOptions_DepthwiseConv2DOptions) {
    auto o = op->builtin_options_as_DepthwiseConv2DOptions();
    ss << tab
       << format(
            "fused_activation_function: %s,",
            tflite::EnumNamesActivationFunctionType()[o->fused_activation_function()])
       << std::endl;
    ss << tab << format("padding: %s,", tflite::EnumNamesPadding()[o->padding()]) << std::endl;
    ss << tab << format("depth_multiplier: %d,", o->depth_multiplier()) << std::endl;
    ss << tab << format("stride_w: %d,", o->stride_w()) << std::endl;
    ss << tab << format("stride_h: %d,", o->stride_h()) << std::endl;
    ss << tab << format("dilation_w_factor: %d,", o->dilation_w_factor()) << std::endl;
    ss << tab << format("dilation_h_factor: %d,", o->dilation_h_factor()) << std::endl;
  } else if (op->builtin_options_type() == tflite::BuiltinOptions::BuiltinOptions_AddOptions) {
    auto o = op->builtin_options_as_AddOptions();
    ss << tab
       << format(
            "fused_activation_function: %s,",
            tflite::EnumNamesActivationFunctionType()[o->fused_activation_function()])
       << std::endl;
  } else if (op->builtin_options_type() == tflite::BuiltinOptions::BuiltinOptions_NONE) {
    // nothing to add.
  } else {
    ss << tab
       << format(
            "!!!: MISSING PRINTING FOR TYPE: %s,",
            tflite::EnumNamesBuiltinOptions()[op->builtin_options_type()])
       << std::endl;
  }
  return ss.str();
}

String replaceStrChar(String str, const String &replace, char ch) {
  size_t found = str.find_first_of(replace);
  while (found != String::npos) {
    str[found] = ch;
    found = str.find_first_of(replace, found + 1);
  }
  return str;
}

}  // namespace

String tensorStr(int idx, const tflite::SubGraph *g, int indent) {
  const auto *t = (*g->tensors())[idx];
  std::stringstream ss;
  ss << format(
    "{idx: %d, name: %s, shape: [%s], type: %s, has_buffer: %d",
    idx,
    t->name() == nullptr ? "null" : t->name()->c_str(),
    strJoin(t->shape()->begin(), t->shape()->end(), "][").c_str(),
    tflite::EnumNamesTensorType()[t->type()],
    t->buffer() != 0);

  if (t->is_variable()) {
    ss << format(", is_variable: %d", t->is_variable());
  }
  if (t->quantization() != nullptr) {
    ss << ", quantization: non-null!!!";
  }
  if (t->sparsity() != nullptr) {
    ss << ", sparsity: non-null!!!";
  }
  if (t->shape_signature() != nullptr) {
    ss << ", shape_signature: non-null!!!";
  }

  ss << "}";
  return ss.str();
}

String tensorVecStr(const flatbuffers::Vector<int> *vec, const tflite::SubGraph *g, int level) {
  if (!vec) {
    return "null";
  }
  std::stringstream ss;
  ss << "[" << std::endl;
  for (int i = 0; i < vec->size(); ++i) {
    ss << format(
      "%s%d: %s,", indent(level + 1).c_str(), i, tensorStr((*vec)[i], g, level + 1).c_str())
       << std::endl;
  }
  ss << indent(level) << "]";
  return ss.str();
}

String opcodeStr(const tflite::OperatorCode *opcode, int level) {
  String customeCodeStr = opcode->custom_code() == nullptr
    ? ""
    : format("custom_code: \"%s\", ", opcode->custom_code()->c_str());
  return format(
    "{%sversion: %d, builtin_code: %s}",
    customeCodeStr.c_str(),
    opcode->version(),
    tflite::EnumNamesBuiltinOperator()[opcode->builtin_code()]);
}

String opcodeVecStr(
  const flatbuffers::Vector<flatbuffers::Offset<tflite::OperatorCode>> *vec, int level) {
  if (!vec) {
    return "null";
  }
  std::stringstream ss;
  ss << "[" << std::endl;
  for (int i = 0; i < vec->size(); ++i) {
    ss << format("%s%d: %s,", indent(level + 1).c_str(), i, opcodeStr((*vec)[i], level + 2).c_str())
       << std::endl;
  }
  ss << indent(level) << "]";
  return ss.str();
}

String metadataStr(const tflite::Metadata *metadata, const tflite::Model *model, int level) {
  return format(
    "{name: %s, buffer: %s}",
    metadata->name() == nullptr ? "null" : metadata->name()->c_str(),
    (*model->buffers())[(*model->metadata_buffer())[metadata->buffer()]]);
}

String metadataVecStr(
  const flatbuffers::Vector<flatbuffers::Offset<tflite::Metadata>> *vec,
  const tflite::Model *model,
  int level) {
  if (!vec) {
    return "null";
  }
  std::stringstream ss;
  ss << "[" << std::endl;
  for (int i = 0; i < vec->size(); ++i) {
    ss << format(
      "%s%d: %s,", indent(level + 1).c_str(), i, metadataStr((*vec)[i], model, level + 1).c_str())
       << std::endl;
  }
  ss << indent(level) << "]";
  return ss.str();
}

// We need to pass in the parent for type unpacking.
String builtinOptionsStr(const tflite::Operator *op, int level) {
  if (op->builtin_options() == nullptr) {
    return "null";
  }

  auto tab = indent(level + 1);

  std::stringstream ss;
  ss << "{" << std::endl;
  ss << tab << format("type: %s,", tflite::EnumNamesBuiltinOptions()[op->builtin_options_type()])
     << std::endl;
  ss << builtinOptionsContents(op, level + 1);
  ss << indent(level) << "}";
  return ss.str();
}

String operatorStr(
  const tflite::Operator *op, const tflite::SubGraph *g, const tflite::Model *m, int level) {
  std::stringstream ss;

  auto opcode = (*m->operator_codes())[op->opcode_index()];
  auto tab = indent(level + 1);
  ss << "{" << std::endl;
  ss << tab << format("op_code: %s,", tflite::EnumNamesBuiltinOperator()[opcode->builtin_code()])
     << std::endl;
  ss << tab << format("builtin_options: %s,", builtinOptionsStr(op, level + 1).c_str())
     << std::endl;
  ss << tab
     << format("custom_options:  %s,", op->custom_options() == nullptr ? "null" : "!!!non-null")
     << std::endl;
  ss << tab
     << format(
          "mutating_variable_inputs:  %s,",
          op->mutating_variable_inputs() == nullptr ? "null" : "!!!non-null")
     << std::endl;

  ss << tab << format("inputs: %s,", tensorVecStr(op->inputs(), g, level + 1).c_str()) << std::endl;
  ss << tab << format("outputs: %s,", tensorVecStr(op->outputs(), g, level + 1).c_str())
     << std::endl;
  ss << tab << format("intermediates: %s,", tensorVecStr(op->intermediates(), g, level + 1).c_str())
     << std::endl;
  ss << indent(level) << "}";
  return ss.str();
}

String operatorVecStr(
  const flatbuffers::Vector<flatbuffers::Offset<tflite::Operator>> *vec,
  const tflite::SubGraph *g,
  const tflite::Model *m,
  int level) {
  if (!vec) {
    return "null";
  }
  std::stringstream ss;
  ss << "[" << std::endl;
  for (int i = 0; i < vec->size(); ++i) {
    ss << format(
      "%s%d: %s,", indent(level + 1).c_str(), i, operatorStr((*vec)[i], g, m, level + 1).c_str())
       << std::endl;
  }
  ss << indent(level) << "]";
  return ss.str();
}

String subgraphStr(const tflite::SubGraph *g, const tflite::Model *m, int level) {
  std::stringstream ss;
  auto tab = indent(level + 1);
  ss << "{" << std::endl;
  ss << tab << format("name: %s,", g->name()->c_str()) << std::endl;
  ss << tab << format("tensors (size): %d,", g->tensors()->size()) << std::endl;
  ss << tab << format("inputs: %s,", tensorVecStr(g->inputs(), g, level + 1).c_str()) << std::endl;
  ss << tab << format("outputs: %s,", tensorVecStr(g->outputs(), g, level + 1).c_str())
     << std::endl;
  ss << tab << format("operators: %s,", operatorVecStr(g->operators(), g, m, level + 1).c_str())
     << std::endl;
  ss << indent(level) << "}";
  return ss.str();
}

String subgraphVecStr(
  const flatbuffers::Vector<flatbuffers::Offset<tflite::SubGraph>> *vec,
  const tflite::Model *m,
  int level) {
  if (!vec) {
    return "null";
  }
  std::stringstream ss;
  ss << "[" << std::endl;
  for (int i = 0; i < vec->size(); ++i) {
    ss << format(
      "%s%d: %s,", indent(level + 1).c_str(), i, subgraphStr((*vec)[i], m, level + 1).c_str())
       << std::endl;
  }
  ss << indent(level) << "]";
  return ss.str();
}

String modelStr(const tflite::Model *m, int level) {
  std::stringstream ss;
  auto tab = indent(level + 1);
  ss << "{" << std::endl;
  ss << tab << format("version: %d,", m->version()) << std::endl;
  ss << tab << format("description: %s,", m->description()->c_str()) << std::endl;
  ss << tab << format("buffers [size]: %d,", m->buffers()->size()) << std::endl;
  ss << tab << format("metadata_buffer [size]: %d,", m->metadata_buffer()->size()) << std::endl;
  ss << tab << format("metadata: %s,", metadataVecStr(m->metadata(), m, level + 1).c_str())
     << std::endl;
  ss << tab << format("operator_codes: %s,", opcodeVecStr(m->operator_codes(), level + 1).c_str())
     << std::endl;
  ss << tab << format("subgraphs: %s,", subgraphVecStr(m->subgraphs(), m, level + 1).c_str())
     << std::endl;
  ss << indent(level) << "}";
  return ss.str();
}

TfliteGraphStats getGraphStats(const tflite::SubGraph *g) {
  TfliteGraphStats s;
  s.tensorProducedByOperator.resize(g->tensors()->size(), -1);
  s.tensorProducesOperators.resize(g->tensors()->size());

  for (int i = 0; i < g->operators()->size(); ++i) {
    auto *op = (*g->operators())[i];
    for (auto idx : (*op->outputs())) {
      if (s.tensorProducedByOperator[idx] != -1) {
        C8Log("!!! Tensor %d was produced by %d and %d", idx, s.tensorProducedByOperator[idx]);
      }
      s.tensorProducedByOperator[idx] = i;
    }
    for (auto idx : (*op->inputs())) {
      s.tensorProducesOperators[idx].push_back(i);
    }
  }

  s.tensorDepths.resize(g->tensors()->size(), -1);
  int maxDepth = 0;
  for (int i = 0; i < g->tensors()->size(); ++i) {
    int depth = depthOfTensor(i, s.tensorDepths, g, s.tensorProducedByOperator);
    if (depth > maxDepth) {
      maxDepth = depth;
    }
  }

  s.tensorsAtLevel.resize(maxDepth + 1);
  for (int i = 0; i < s.tensorDepths.size(); ++i) {
    s.tensorsAtLevel[s.tensorDepths[i]].push_back(i);
  }

  return s;
}

String getGraphVizString(const tflite::SubGraph *g, const tflite::Model *m) {
  std::stringstream ss;
  // A graph-viz directed graph.
  ss << "digraph g {" << std::endl;
  auto s = getGraphStats(g);
  bool firstLevel = true;

  // Make input nodes for the graph's inputs.
  for (const auto tidx : *g->inputs()) {
    auto *t = (*g->tensors())[tidx];
    ss << format(
      "input%03d [shape=record label=\"%s\\n[%s]\"];",
      tidx,
      t->name()->c_str(),
      strJoin(t->shape()->begin(), t->shape()->end(), "][").c_str())
       << std::endl;
  }

  // Iterate over the operator levels in order.
  for (const auto &l : s.tensorsAtLevel) {
    if (firstLevel) {
      // Don't print level 0 which has all the model data; we'll inject model data to each operator
      // as it's used.
      firstLevel = false;
      continue;
    }

    // Operators that are on the same level
    Vector<String> sameLevel;

    // Data tensor info that can be considered as a parameter to an operator.
    Vector<String> params;

    // For each tensor at this level, print its node and edges going into it.
    for (auto tidx : l) {
      const auto *t = (*g->tensors())[tidx];
      String name = t->name()->c_str();
      auto opidx = s.tensorProducedByOperator[tidx];
      String opName = format("op%03d", opidx);  // This is the node name.

      // Mark operators at the same level so we can mark them rank=same.
      sameLevel.push_back(opName);
      const auto *op = (*g->operators())[opidx];

      Vector<String> opInputs;
      Vector<String> opParams;
      for (auto iidx : *op->inputs()) {
        const auto *it = (*g->tensors())[iidx];
        String iname = it->name()->c_str();
        if (s.tensorProducesOperators[iidx].size() == 1 && it->buffer() > 0) {
          // For primary inputs (like convolution kernels, etc.), if they are tied to a single
          // a single operator, we want them to appear like parameters to that operator.
          opParams.push_back(format(
            "%s\\n[%s]\\n--",
            iname.c_str(),
            strJoin(it->shape()->begin(), it->shape()->end(), "][").c_str()));
        } else if (s.tensorProducedByOperator[iidx] == -1) {
          // Otherwise, if this input has no data and is not produced, it's an input, so make it an
          // input node.
          opInputs.push_back(format("input%03d", iidx));
        } else {
          // Otherwise, another operator is the source node.
          opInputs.push_back(format("op%03d", s.tensorProducedByOperator[iidx]));
        }
      }

      // Output this operator as a record node with metadata.
      ss << opName << " [shape=record label=\"{";

      // On the left side, print the data tensors that are tied to this operation.
      ss << strJoin(opParams.begin(), opParams.end(), "\\n");
      auto opcode = (*m->operator_codes())[op->opcode_index()];

      // In the middle, print the operator code, output tensor name, and output tensor dimensions.
      String opCode = tflite::EnumNamesBuiltinOperator()[opcode->builtin_code()];
      ss << "}|{" << opCode << "|" << name << "|["
         << strJoin(t->shape()->begin(), t->shape()->end(), "][") << "]}|{";

      // On the right, print the operator options.
      std::istringstream opOptions(builtinOptionsContents(op, 0));
      for (String line; std::getline(opOptions, line);) {
        ss << line << "\\n";
      }
      ss << "}\"];" << std::endl;

      // Now print the edge from either the input or a previous operator.
      for (const auto &iname : opInputs) {
        ss << iname << "->" << opName << ";" << std::endl;
      }
    }

    // Keep all operators at the same graph level at the same rank.
    if (sameLevel.size() > 1) {
      ss << "{rank=same; ";
      for (const auto &n : sameLevel) {
        ss << n << " ";
      }
      ss << "}" << std::endl;
    }
  }

  // End the digraph.
  ss << "}" << std::endl;

  // Some tensor names have '/' in them, which graphviz doesn't like. Replace them with "_".
  return replaceStrChar(ss.str(), "/", '_');
}

}  // namespace c8
