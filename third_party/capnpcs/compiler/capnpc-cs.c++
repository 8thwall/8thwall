// Copyright (c) 2013-2014 Sandstorm Development Group, Inc. and contributors
// Licensed under the MIT License:
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.

// This program is a code generator plugin for `capnp compile` which generates java code.
// It is a modified version of the C++ code generator plugin, capnpc-c++.

#include <capnp/dynamic.h>
#include <capnp/schema-loader.h>
#include <capnp/schema.capnp.h>
#include <capnp/serialize.h>
#include <errno.h>
#include <fcntl.h>
#include <kj/debug.h>
#include <kj/io.h>
#include <kj/main.h>
#include <kj/string-tree.h>
#include <kj/vector.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <algorithm>
#include <set>
#include <unordered_map>
#include <unordered_set>

#if HAVE_CONFIG_H
#include "config.h"
#endif

#if CAPNP_VERSION < 5000
#error "This version of capnpc-cs requires Cap'n Proto version 0.5 or higher."
#endif

#ifndef VERSION
#define VERSION "(unknown)"
#endif

#if _WIN32
#define mkdir(path, mode) mkdir(path)
#endif

namespace capnp {
namespace {

static constexpr uint64_t CS_NAMESPACE_ANNOTATION_ID = 0xf7e6bf7a1b2c378bull;
static constexpr uint64_t CC_NAMESPACE_ANNOTATION_ID = 0xb9c6f99ebf805f2cull;

static constexpr const char *FIELD_SIZE_NAMES[] = {
  "VOID", "BIT", "BYTE", "TWO_BYTES", "FOUR_BYTES", "EIGHT_BYTES", "POINTER", "INLINE_COMPOSITE"};

bool hasDiscriminantValue(const schema::Field::Reader &reader) {
  return reader.getDiscriminantValue() != schema::Field::NO_DISCRIMINANT;
}

void enumerateDeps(schema::Type::Reader type, std::set<uint64_t> &deps) {
  switch (type.which()) {
    case schema::Type::STRUCT:
      deps.insert(type.getStruct().getTypeId());
      break;
    case schema::Type::ENUM:
      deps.insert(type.getEnum().getTypeId());
      break;
    case schema::Type::INTERFACE:
      deps.insert(type.getInterface().getTypeId());
      break;
    case schema::Type::LIST:
      enumerateDeps(type.getList().getElementType(), deps);
      break;
    default:
      break;
  }
}

void enumerateDeps(schema::Node::Reader node, std::set<uint64_t> &deps) {
  switch (node.which()) {
    case schema::Node::STRUCT: {
      auto structNode = node.getStruct();
      for (auto field : structNode.getFields()) {
        switch (field.which()) {
          case schema::Field::SLOT:
            enumerateDeps(field.getSlot().getType(), deps);
            break;
          case schema::Field::GROUP:
            deps.insert(field.getGroup().getTypeId());
            break;
        }
      }
      if (structNode.getIsGroup()) {
        deps.insert(node.getScopeId());
      }
      break;
    }
    case schema::Node::INTERFACE: {
      auto interfaceNode = node.getInterface();
      for (auto superclass : interfaceNode.getSuperclasses()) {
        deps.insert(superclass.getId());
      }
      for (auto method : interfaceNode.getMethods()) {
        deps.insert(method.getParamStructType());
        deps.insert(method.getResultStructType());
      }
      break;
    }
    default:
      break;
  }
}

struct OrderByName {
  template <typename T>
  inline bool operator()(const T &a, const T &b) const {
    return a.getProto().getName() < b.getProto().getName();
  }
};

template <typename MemberList>
kj::Array<uint> makeMembersByName(MemberList &&members) {
  auto sorted = KJ_MAP(member, members) { return member; };
  std::sort(sorted.begin(), sorted.end(), OrderByName());
  return KJ_MAP(member, sorted) { return member.getIndex(); };
}

kj::StringPtr baseName(kj::StringPtr path) {
  KJ_IF_MAYBE(slashPos, path.findLast('/')) { return path.slice(*slashPos + 1); }
  else {
    return path;
  }
}

kj::String safeIdentifier(kj::StringPtr identifier) {
  // Given a desired identifier name, munge it to make it safe for use in generated code.
  //
  // If the identifier is a keyword, this adds an underscore to the end.

  static const std::set<kj::StringPtr> keywords({"alignas",      "alignof",
                                                 "and",          "and_eq",
                                                 "asm",          "auto",
                                                 "bitand",       "bitor",
                                                 "bool",         "break",
                                                 "case",         "catch",
                                                 "char",         "char16_t",
                                                 "char32_t",     "class",
                                                 "compl",        "const",
                                                 "constexpr",    "const_cast",
                                                 "continue",     "decltype",
                                                 "default",      "delete",
                                                 "do",           "double",
                                                 "dynamic_cast", "else",
                                                 "enum",         "explicit",
                                                 "export",       "extern",
                                                 "false",        "float",
                                                 "for",          "friend",
                                                 "goto",         "if",
                                                 "inline",       "int",
                                                 "long",         "mutable",
                                                 "namespace",    "new",
                                                 "noexcept",     "not",
                                                 "not_eq",       "nullptr",
                                                 "operator",     "or",
                                                 "or_eq",        "private",
                                                 "protected",    "public",
                                                 "register",     "reinterpret_cast",
                                                 "return",       "short",
                                                 "signed",       "sizeof",
                                                 "static",       "static_assert",
                                                 "static_cast",  "struct",
                                                 "switch",       "template",
                                                 "this",         "thread_local",
                                                 "throw",        "true",
                                                 "try",          "typedef",
                                                 "typeid",       "typename",
                                                 "union",        "unsigned",
                                                 "using",        "virtual",
                                                 "void",         "volatile",
                                                 "wchar_t",      "while",
                                                 "xor",          "xor_eq"});

  if (keywords.count(identifier) > 0) {
    return kj::str(identifier, '_');
  } else {
    return kj::heapString(identifier);
  }
}

kj::String spaces(int n) { return kj::str(kj::repeat(' ', n * 2)); }

// =======================================================================================

class CapnpcJavaMain {
public:
  CapnpcJavaMain(kj::ProcessContext &context) : context(context) {}

  kj::MainFunc getMain() {
    return kj::MainBuilder(
             context,
             "Cap'n Proto CS plugin version " VERSION,
             "This is a Cap'n Proto compiler plugin which generates CS code."
             " This is meant to be run using the Cap'n Proto compiler, e.g.:\n"
             "    capnp compile -ocs foo.capnp")
      .callAfterParsing(KJ_BIND_METHOD(*this, run))
      .build();
  }

private:
  kj::ProcessContext &context;
  SchemaLoader schemaLoader;
  std::unordered_set<uint64_t> usedImports;
  bool hasInterfaces = false;


  kj::String getCsNamespace(schema::Node::Reader node) {
    if (node.getScopeId() != 0) {
      return kj::str("");
    }

    kj::String package;
    kj::String fallbackPackage;

    for (auto annotation : node.getAnnotations()) {
      if (annotation.getId() == CS_NAMESPACE_ANNOTATION_ID) {
        package = kj::str(annotation.getValue().getText());
      }
      if (annotation.getId() == CC_NAMESPACE_ANNOTATION_ID) {
        package = kj::str(annotation.getValue().getText());
      }
    }
    return package.size() == 0 ? kj::mv(fallbackPackage) : kj::mv(package);
  }

  kj::StringTree javaFullName(Schema schema) {
    auto node = schema.getProto();
    if (node.getScopeId() == 0) {
      usedImports.insert(node.getId());
      return getCsNamespace(node);
    } else {
      Schema parent = schemaLoader.get(node.getScopeId());
      for (auto nested : parent.getProto().getNestedNodes()) {
        if (nested.getId() == node.getId()) {
          return kj::strTree(javaFullName(parent), ".", nested.getName());
        }
      }
      KJ_FAIL_REQUIRE("A schema Node's supposed scope did not contain the node as a NestedNode.");
    }
  }

  kj::Vector<kj::String> getTypeParameters(Schema schema) {
    auto node = schema.getProto();
    kj::Vector<kj::String> result;
    if (node.getScopeId() != 0) {
      Schema parent = schemaLoader.get(node.getScopeId());
      result = getTypeParameters(parent);
    }
    for (auto parameter : node.getParameters()) {
      result.add(kj::str(parameter.getName(), "_", kj::hex(node.getId())));
    }
    return kj::mv(result);
  }

  kj::Vector<kj::String> getTypeArguments(Schema leaf, Schema schema, kj::String suffix) {
    auto node = schema.getProto();
    kj::Vector<kj::String> result;
    if (node.getScopeId() != 0) {
      Schema parent = schemaLoader.get(node.getScopeId());
      result = getTypeArguments(leaf, parent, kj::str(suffix));
    }
    auto brandArguments = leaf.getBrandArgumentsAtScope(node.getId());
    auto parameters = node.getParameters();
    for (int ii = 0; ii < parameters.size(); ++ii) {
      result.add(typeName(brandArguments[ii], kj::str(suffix)).flatten());
    }
    return kj::mv(result);
  }

  kj::Vector<kj::String> getFactoryArguments(Schema leaf, Schema schema) {
    auto node = schema.getProto();
    kj::Vector<kj::String> result;
    if (node.getScopeId() != 0) {
      Schema parent = schemaLoader.get(node.getScopeId());
      result = getFactoryArguments(leaf, parent);
    }
    auto brandArguments = leaf.getBrandArgumentsAtScope(node.getId());
    auto parameters = node.getParameters();
    for (int ii = 0; ii < parameters.size(); ++ii) {
      result.add(makeFactoryArg(brandArguments[ii]));
    }
    return kj::mv(result);
  }

  kj::String toUpperCase(kj::StringPtr name) {
    kj::Vector<char> result(name.size() + 4);

    for (char c : name) {
      if ('a' <= c && c <= 'z') {
        result.add(c - 'a' + 'A');
      } else if (result.size() > 0 && 'A' <= c && c <= 'Z') {
        result.add('_');
        result.add(c);
      } else {
        result.add(c);
      }
    }

    result.add('\0');

    return kj::String(result.releaseAsArray());
  }

  kj::String toTitleCase(kj::StringPtr name) {
    kj::String result = kj::heapString(name);
    if ('a' <= result[0] && result[0] <= 'z') {
      result[0] = result[0] - 'a' + 'A';
    }
    return kj::mv(result);
  }

  kj::StringTree typeName(capnp::Type type, kj::String suffix = nullptr) {
    switch (type.which()) {
      case schema::Type::VOID:
        return kj::strTree("Capnp.Void");

      case schema::Type::BOOL:
        return kj::strTree("bool");
      case schema::Type::INT8:
        return kj::strTree("byte");
      case schema::Type::INT16:
        return kj::strTree("short");
      case schema::Type::INT32:
        return kj::strTree("int");
      case schema::Type::INT64:
        return kj::strTree("long");
      case schema::Type::UINT8:
        return kj::strTree("byte");
      case schema::Type::UINT16:
        return kj::strTree("short");
      case schema::Type::UINT32:
        return kj::strTree("int");
      case schema::Type::UINT64:
        return kj::strTree("long");
      case schema::Type::FLOAT32:
        return kj::strTree("float");
      case schema::Type::FLOAT64:
        return kj::strTree("double");

      case schema::Type::TEXT:
        return kj::strTree("Capnp.Text.", suffix);
      case schema::Type::DATA:
        return kj::strTree("Capnp.Data.", suffix);

      case schema::Type::ENUM:
        return javaFullName(type.asEnum());
      case schema::Type::STRUCT: {
        auto structSchema = type.asStruct();
        if (structSchema.getProto().getIsGeneric()) {
          auto typeArgs = getTypeArguments(structSchema, structSchema, kj::str(suffix));
          return kj::strTree(
            javaFullName(structSchema),
            ".",
            suffix,
            "<",
            kj::StringTree(KJ_MAP(arg, typeArgs) { return kj::strTree(arg); }, ", "),
            ">");
        } else {
          return kj::strTree(javaFullName(type.asStruct()), ".", suffix);
        }
      }
      case schema::Type::INTERFACE:
        return javaFullName(type.asInterface());

      case schema::Type::LIST: {
        auto elementType = type.asList().getElementType();
        switch (elementType.which()) {
          case schema::Type::VOID:
            return kj::strTree("Capnp.PrimitiveList.Void.", suffix);
          case schema::Type::BOOL:
            return kj::strTree("Capnp.PrimitiveList.Boolean.", suffix);
          case schema::Type::INT8:
          case schema::Type::UINT8:
            return kj::strTree("Capnp.PrimitiveList.Byte.", suffix);
          case schema::Type::INT16:
          case schema::Type::UINT16:
            return kj::strTree("Capnp.PrimitiveList.Short.", suffix);
          case schema::Type::INT32:
          case schema::Type::UINT32:
            return kj::strTree("Capnp.PrimitiveList.Int.", suffix);
          case schema::Type::INT64:
          case schema::Type::UINT64:
            return kj::strTree("Capnp.PrimitiveList.Long.", suffix);
          case schema::Type::FLOAT32:
            return kj::strTree("Capnp.PrimitiveList.Float.", suffix);
          case schema::Type::FLOAT64:
            return kj::strTree("Capnp.PrimitiveList.Double.", suffix);
          case schema::Type::STRUCT: {
            auto inner = typeName(elementType, kj::str(suffix));
            return kj::strTree("Capnp.StructList.", suffix, "<", kj::mv(inner), ">");
          }
          case schema::Type::TEXT:
            return kj::strTree("Capnp.TextList.", suffix);
          case schema::Type::DATA:
            return kj::strTree("Capnp.DataList.", suffix);
          case schema::Type::ENUM: {
            auto inner = typeName(elementType, kj::str(suffix));
            return kj::strTree("Capnp.EnumList.", suffix, "<", kj::mv(inner), ">");
          }
          case schema::Type::LIST: {
            auto inner = typeName(elementType, kj::str(suffix));
            return kj::strTree("Capnp.ListList.", suffix, "<", kj::mv(inner), ">");
          }
          case schema::Type::INTERFACE:
          case schema::Type::ANY_POINTER:
            KJ_FAIL_REQUIRE("unimplemented");
        }
        KJ_UNREACHABLE;
      }
      case schema::Type::ANY_POINTER: {
        KJ_IF_MAYBE(brandParam, type.getBrandParameter()) {
          return kj::strTree(
            schemaLoader.get(brandParam->scopeId)
              .getProto()
              .getParameters()[brandParam->index]
              .getName(),
            "_",
            kj::hex(brandParam->scopeId),
            "_",
            suffix);
        }
        else {
          return kj::strTree("Capnp.AnyPointer.", suffix);
        }
      }
    }
    KJ_UNREACHABLE;
  }

  kj::StringTree literalValue(schema::Type::Reader type, schema::Value::Reader value) {
    switch (value.which()) {
      case schema::Value::VOID:
        return kj::strTree("Capnp.Void.VOID");
      case schema::Value::BOOL:
        return kj::strTree(value.getBool() ? "true" : "false");
      case schema::Value::INT8:
        return kj::strTree(value.getInt8());
      case schema::Value::INT16:
        return kj::strTree(value.getInt16());
      case schema::Value::INT32:
        return kj::strTree(value.getInt32());
      case schema::Value::INT64:
        return kj::strTree(value.getInt64(), "L");
      case schema::Value::UINT8:
        return kj::strTree(kj::implicitCast<int8_t>(value.getUint8()));
      case schema::Value::UINT16:
        return kj::strTree(kj::implicitCast<int16_t>(value.getUint16()));
      case schema::Value::UINT32:
        return kj::strTree(kj::implicitCast<int32_t>(value.getUint32()));
      case schema::Value::UINT64:
        return kj::strTree(kj::implicitCast<int64_t>(value.getUint64()), "L");
      case schema::Value::FLOAT32:
        return kj::strTree(value.getFloat32(), "f");
      case schema::Value::FLOAT64:
        return kj::strTree(value.getFloat64());
      case schema::Value::ENUM: {
        EnumSchema schema = schemaLoader.get(type.getEnum().getTypeId()).asEnum();
        if (value.getEnum() < schema.getEnumerants().size()) {
          return kj::strTree(
            javaFullName(schema),
            ".",
            toUpperCase(schema.getEnumerants()[value.getEnum()].getProto().getName()));
        } else {
          return kj::strTree("static_cast<", javaFullName(schema), ">(", value.getEnum(), ")");
        }
      }

      case schema::Value::TEXT:
      case schema::Value::DATA:
      case schema::Value::STRUCT:
      case schema::Value::INTERFACE:
      case schema::Value::LIST:
      case schema::Value::ANY_POINTER:
        KJ_FAIL_REQUIRE("literalValue() can only be used on primitive types.");
    }
    KJ_UNREACHABLE;
  }

  // -----------------------------------------------------------------
  // Code to deal with "slots" -- determines what to zero out when we clear a group.

  static uint typeSizeBits(schema::Type::Which whichType) {
    switch (whichType) {
      case schema::Type::BOOL:
        return 1;
      case schema::Type::INT8:
        return 8;
      case schema::Type::INT16:
        return 16;
      case schema::Type::INT32:
        return 32;
      case schema::Type::INT64:
        return 64;
      case schema::Type::UINT8:
        return 8;
      case schema::Type::UINT16:
        return 16;
      case schema::Type::UINT32:
        return 32;
      case schema::Type::UINT64:
        return 64;
      case schema::Type::FLOAT32:
        return 32;
      case schema::Type::FLOAT64:
        return 64;
      case schema::Type::ENUM:
        return 16;

      case schema::Type::VOID:
      case schema::Type::TEXT:
      case schema::Type::DATA:
      case schema::Type::LIST:
      case schema::Type::STRUCT:
      case schema::Type::INTERFACE:
      case schema::Type::ANY_POINTER:
        KJ_FAIL_REQUIRE("Should only be called for data types.");
    }
    KJ_UNREACHABLE;
  }

  enum class Section { NONE, DATA, POINTERS };

  static Section sectionFor(schema::Type::Which whichType) {
    switch (whichType) {
      case schema::Type::VOID:
        return Section::NONE;
      case schema::Type::BOOL:
      case schema::Type::INT8:
      case schema::Type::INT16:
      case schema::Type::INT32:
      case schema::Type::INT64:
      case schema::Type::UINT8:
      case schema::Type::UINT16:
      case schema::Type::UINT32:
      case schema::Type::UINT64:
      case schema::Type::FLOAT32:
      case schema::Type::FLOAT64:
      case schema::Type::ENUM:
        return Section::DATA;
      case schema::Type::TEXT:
      case schema::Type::DATA:
      case schema::Type::LIST:
      case schema::Type::STRUCT:
      case schema::Type::INTERFACE:
      case schema::Type::ANY_POINTER:
        return Section::POINTERS;
    }
    KJ_UNREACHABLE;
  }

  static kj::StringPtr maskType(schema::Type::Which whichType) {
    switch (whichType) {
      case schema::Type::BOOL:
        return "bool";
      case schema::Type::INT8:
        return "byte";
      case schema::Type::INT16:
        return "short";
      case schema::Type::INT32:
        return "int";
      case schema::Type::INT64:
        return "long";
      case schema::Type::UINT8:
        return "byte";
      case schema::Type::UINT16:
        return "short";
      case schema::Type::UINT32:
        return "int";
      case schema::Type::UINT64:
        return "long";
      case schema::Type::FLOAT32:
        return "int";
      case schema::Type::FLOAT64:
        return "long";
      case schema::Type::ENUM:
        return "short";

      case schema::Type::VOID:
      case schema::Type::TEXT:
      case schema::Type::DATA:
      case schema::Type::LIST:
      case schema::Type::STRUCT:
      case schema::Type::INTERFACE:
      case schema::Type::ANY_POINTER:
        KJ_FAIL_REQUIRE("Should only be called for data types.");
    }
    KJ_UNREACHABLE;
  }

  static kj::StringPtr maskZeroLiteral(schema::Type::Which whichType) {
    switch (whichType) {
      case schema::Type::BOOL:
        return "false";
      case schema::Type::INT8:
        return "(byte)0";
      case schema::Type::INT16:
        return "(short)0";
      case schema::Type::INT32:
        return "0";
      case schema::Type::INT64:
        return "0L";
      case schema::Type::UINT8:
        return "(byte)0";
      case schema::Type::UINT16:
        return "(short)0";
      case schema::Type::UINT32:
        return "0";
      case schema::Type::UINT64:
        return "0L";
      case schema::Type::FLOAT32:
        return "0";
      case schema::Type::FLOAT64:
        return "0L";
      case schema::Type::ENUM:
        return "(short)0";

      case schema::Type::VOID:
      case schema::Type::TEXT:
      case schema::Type::DATA:
      case schema::Type::LIST:
      case schema::Type::STRUCT:
      case schema::Type::INTERFACE:
      case schema::Type::ANY_POINTER:
        KJ_FAIL_REQUIRE("Should only be called for data types.");
    }
    KJ_UNREACHABLE;
  }

  struct Slot {
    schema::Type::Which whichType;
    uint offset;

    bool isSupersetOf(Slot other) const {
      auto section = sectionFor(whichType);
      if (section != sectionFor(other.whichType))
        return false;
      switch (section) {
        case Section::NONE:
          return true;  // all voids overlap
        case Section::DATA: {
          auto bits = typeSizeBits(whichType);
          auto start = offset * bits;
          auto otherBits = typeSizeBits(other.whichType);
          auto otherStart = other.offset * otherBits;
          return start <= otherStart && otherStart + otherBits <= start + bits;
        }
        case Section::POINTERS:
          return offset == other.offset;
      }
      KJ_UNREACHABLE;
    }

    bool operator<(Slot other) const {
      // Sort by section, then start position, and finally size.

      auto section = sectionFor(whichType);
      auto otherSection = sectionFor(other.whichType);
      if (section < otherSection) {
        return true;
      } else if (section > otherSection) {
        return false;
      }

      switch (section) {
        case Section::NONE:
          return false;
        case Section::DATA: {
          auto bits = typeSizeBits(whichType);
          auto start = offset * bits;
          auto otherBits = typeSizeBits(other.whichType);
          auto otherStart = other.offset * otherBits;
          if (start < otherStart) {
            return true;
          } else if (start > otherStart) {
            return false;
          }

          // Sort larger sizes before smaller.
          return bits > otherBits;
        }
        case Section::POINTERS:
          return offset < other.offset;
      }
      KJ_UNREACHABLE;
    }
  };

  void getSlots(StructSchema schema, kj::Vector<Slot> &slots) {
    auto structProto = schema.getProto().getStruct();
    if (structProto.getDiscriminantCount() > 0) {
      slots.add(Slot{schema::Type::UINT16, structProto.getDiscriminantOffset()});
    }

    for (auto field : schema.getFields()) {
      auto proto = field.getProto();
      switch (proto.which()) {
        case schema::Field::SLOT: {
          auto slot = proto.getSlot();
          slots.add(Slot{slot.getType().which(), slot.getOffset()});
          break;
        }
        case schema::Field::GROUP:
          getSlots(field.getType().asStruct(), slots);
          break;
      }
    }
  }

  kj::Array<Slot> getSortedSlots(StructSchema schema) {
    // Get a representation of all of the field locations owned by this schema, e.g. so that they
    // can be zero'd out.

    kj::Vector<Slot> slots(schema.getFields().size());
    getSlots(schema, slots);
    std::sort(slots.begin(), slots.end());

    kj::Vector<Slot> result(slots.size());

    // All void slots are redundant, and they sort towards the front of the list.  By starting out
    // with `prevSlot` = void, we will end up skipping them all, which is what we want.
    Slot prevSlot = {schema::Type::VOID, 0};
    for (auto slot : slots) {
      if (prevSlot.isSupersetOf(slot)) {
        // This slot is redundant as prevSlot is a superset of it.
        continue;
      }

      // Since all sizes are power-of-two, if two slots overlap at all, one must be a superset of
      // the other.  Since we sort slots by starting position, we know that the only way `slot`
      // could be a superset of `prevSlot` is if they have the same starting position.  However,
      // since we sort slots with the same starting position by descending size, this is not
      // possible.
      KJ_DASSERT(!slot.isSupersetOf(prevSlot));

      result.add(slot);

      prevSlot = slot;
    }

    return result.releaseAsArray();
  }

  // -----------------------------------------------------------------

  struct DiscriminantChecks {
    kj::String has;
    kj::String check;
    kj::String set;
    kj::StringTree readerIsDef;
    kj::StringTree builderIsDef;
  };

  DiscriminantChecks makeDiscriminantChecks(
    kj::StringPtr scope, kj::StringPtr memberName, StructSchema containingStruct, int indent) {
    auto discrimOffset = containingStruct.getProto().getStruct().getDiscriminantOffset();

    kj::String titleCase = toTitleCase(memberName);
    kj::String upperCase = toUpperCase(memberName);

    return DiscriminantChecks{
      kj::str(spaces(indent), "  if (which() != ", scope, "Which.", upperCase, ") return false;\n"),
      kj::str(
        spaces(indent),
        "  if(which() != ",
        scope,
        "Which.",
        upperCase,
        ") {\n",
        spaces(indent),
        "    throw new System.ApplicationException(\"Must check which() before get()ing a union member.\");\n",
        spaces(indent),
        "}\n"),
      kj::str(
        spaces(indent),
        "  _setShortField(",
        discrimOffset,
        ", (short)((System.IConvertible)",
        scope,
        "Which.",
        upperCase,
        ").ToInt16(System.Globalization.CultureInfo.CurrentCulture));\n"),
      kj::strTree(
        spaces(indent),
        "public bool is",
        titleCase,
        "() {\n",
        spaces(indent),
        "  return which() == ",
        scope,
        "Which.",
        upperCase,
        ";\n",
        spaces(indent),
        "}\n"),
      kj::strTree(
        spaces(indent),
        "public bool is",
        titleCase,
        "() {\n",
        spaces(indent),
        "  return which() == ",
        scope,
        "Which.",
        upperCase,
        ";\n",
        spaces(indent),
        "}\n")};
  }

  // -----------------------------------------------------------------

  struct FieldText {
    kj::StringTree readerMethodDecls;
    kj::StringTree builderMethodDecls;
  };

  enum class FieldKind { PRIMITIVE, BLOB, STRUCT, LIST, INTERFACE, ANY_POINTER };

  kj::StringTree makeEnumGetter(
    EnumSchema schema, uint offset, kj::String defaultMaskParam, int indent) {
    auto enumerants = schema.getEnumerants();
    return kj::strTree(
      spaces(indent),
      "switch(_getShortField(",
      offset,
      defaultMaskParam,
      ")) {\n",
      KJ_MAP(e, enumerants) {
        return kj::strTree(
          spaces(indent + 1),
          "case ",
          e.getOrdinal(),
          " : return ",
          javaFullName(schema),
          ".",
          toUpperCase(e.getProto().getName()),
          ";\n");
      },
      spaces(indent + 1),
      "default: return ",
      javaFullName(schema),
      "._NOT_IN_SCHEMA;\n",
      spaces(indent),
      "}\n");
  }

  kj::String makeFactoryArg(capnp::Type type) {
    switch (type.which()) {
      case schema::Type::TEXT: {
        return kj::str("Capnp.Text.factory");
      }
      case schema::Type::DATA: {
        return kj::str("Capnp.Data.factory");
      }
      case schema::Type::ANY_POINTER: {
        KJ_IF_MAYBE(brandParam, type.getBrandParameter()) {
          return kj::str(
            schemaLoader.get(brandParam->scopeId)
              .getProto()
              .getParameters()[brandParam->index]
              .getName(),
            "_",
            kj::hex(brandParam->scopeId),
            "_Factory");
        }
        else {
          return kj::str("Capnp.AnyPointer.factory");
        }
      }
      case schema::Type::STRUCT: {
        auto structSchema = type.asStruct();
        auto node = structSchema.getProto();
        if (node.getIsGeneric()) {
          auto factoryArgs = getFactoryArguments(structSchema, structSchema);
          return kj::strTree(
                   javaFullName(structSchema),
                   ".newFactory(",
                   kj::StringTree(KJ_MAP(arg, factoryArgs) { return kj::strTree(arg); }, ","),
                   ")")
            .flatten();
        } else {
          return kj::str(typeName(type, kj::str("factory")));
        }
      }
      case schema::Type::LIST: {
        auto elementType = type.asList().getElementType();
        switch (elementType.which()) {
          case schema::Type::STRUCT: {
            auto elementStructSchema = elementType.asStruct();
            auto elementNode = elementStructSchema.getProto();
            if (elementNode.getIsGeneric()) {
              auto factoryArgs = getFactoryArguments(elementStructSchema, elementStructSchema);
              return kj::strTree(
                       "new Capnp.StructList.Factory<",
                       typeName(elementType, kj::str("Builder")),
                       ", ",
                       typeName(elementType, kj::str("Reader")),
                       ">(",
                       javaFullName(elementStructSchema),
                       ".newFactory(",
                       kj::StringTree(KJ_MAP(arg, factoryArgs) { return kj::strTree(arg); }, ","),
                       "))")
                .flatten();
            } else {
              return kj::str(typeName(elementType, kj::str("listFactory")));
            }
          }
          case schema::Type::LIST:
            return kj::str(
              "new Capnp.ListList.Factory<",
              typeName(elementType, kj::str("Builder")),
              ", ",
              typeName(elementType, kj::str("Reader")),
              ">(",
              makeFactoryArg(elementType),
              ")");
          case schema::Type::ENUM:
            return kj::str(
              "new Capnp.EnumList.Factory<",
              typeName(elementType),
              ">(System.Enum.GetValues(typeof(",
              typeName(elementType, kj::str("")),
              ")))");
          default:
            return kj::str(typeName(type, kj::str("factory")));
        }
      }
      default:
        KJ_UNREACHABLE;
    }
  }

  FieldText makeFieldText(kj::StringPtr scope, StructSchema::Field field, int indent) {
    auto proto = field.getProto();
    kj::String titleCase = toTitleCase(proto.getName());

    DiscriminantChecks unionDiscrim;
    if (hasDiscriminantValue(proto)) {
      unionDiscrim =
        makeDiscriminantChecks(scope, proto.getName(), field.getContainingStruct(), indent + 1);
    }

    switch (proto.which()) {
      case schema::Field::SLOT:
        // Continue below.
        break;

      case schema::Field::GROUP: {
        auto slots =
          getSortedSlots(schemaLoader.get(field.getProto().getGroup().getTypeId()).asStruct());
        return FieldText{
          kj::strTree(
            kj::mv(unionDiscrim.readerIsDef),
            spaces(indent),
            "  public ",
            titleCase,
            ".Reader get",
            titleCase,
            "() {\n",
            spaces(indent),
            "    return new ",
            scope,
            titleCase,
            ".Reader(segment, data, pointers, dataSize, pointerCount, nestingLimit);\n",
            spaces(indent),
            "  }\n",
            "\n"),

          kj::strTree(
            kj::mv(unionDiscrim.builderIsDef),
            spaces(indent),
            "  public ",
            titleCase,
            ".Builder get",
            titleCase,
            "() {\n",
            spaces(indent),
            "    return new ",
            scope,
            titleCase,
            ".Builder(segment, data, pointers, dataSize, pointerCount);\n",
            spaces(indent),
            "  }\n",
            spaces(indent),
            "  public ",
            titleCase,
            ".Builder init",
            titleCase,
            "() {\n",
            unionDiscrim.set,
            KJ_MAP(slot, slots) {
              switch (sectionFor(slot.whichType)) {
                case Section::NONE:
                  return kj::strTree();
                case Section::DATA:
                  return kj::strTree(
                    spaces(indent),
                    "    _set",
                    toTitleCase(maskType(slot.whichType)),
                    "Field(",
                    slot.offset,
                    ",",
                    maskZeroLiteral(slot.whichType),
                    ");\n");
                case Section::POINTERS:
                  return kj::strTree(
                    spaces(indent), "    _clearPointerField(", slot.offset, ");\n");
              }
              KJ_UNREACHABLE;
            },
            "  return new ",
            scope,
            titleCase,
            ".Builder(segment, data, pointers, dataSize, pointerCount);\n",
            spaces(indent),
            "  }\n",
            "\n")};
      }
    }

    auto slot = proto.getSlot();

    FieldKind kind = FieldKind::PRIMITIVE;
    kj::String ownedType;
    kj::String builderType = typeName(field.getType(), kj::str("Builder")).flatten();
    kj::String readerType = typeName(field.getType(), kj::str("Reader")).flatten();
    kj::String defaultMask;    // primitives only
    size_t defaultOffset = 0;  // pointers only: offset of the default value within the schema.
    size_t defaultSize = 0;    // blobs only: byte size of the default value.

    auto typeBody = slot.getType();
    auto defaultBody = slot.getDefaultValue();
    switch (typeBody.which()) {
      case schema::Type::VOID:
        kind = FieldKind::PRIMITIVE;
        break;

#define HANDLE_PRIMITIVE(discrim, typeName, javaTypeName, defaultName, suffix) \
  case schema::Type::discrim:                                                  \
    kind = FieldKind::PRIMITIVE;                                               \
    if (defaultBody.get##defaultName() != 0) {                                 \
      defaultMask = kj::str(                                                   \
        "(",                                                                   \
        #javaTypeName,                                                         \
        ")",                                                                   \
        kj::implicitCast<typeName>(defaultBody.get##defaultName()),            \
        #suffix);                                                              \
    }                                                                          \
    break;

        HANDLE_PRIMITIVE(BOOL, bool, bool, Bool, );
        HANDLE_PRIMITIVE(INT8, ::int8_t, byte, Int8, );
        HANDLE_PRIMITIVE(INT16, ::int16_t, short, Int16, );
        HANDLE_PRIMITIVE(INT32, ::int32_t, int, Int32, );
        HANDLE_PRIMITIVE(INT64, ::int64_t, long, Int64, L);
        HANDLE_PRIMITIVE(UINT8, ::int8_t, byte, Uint8, );
        HANDLE_PRIMITIVE(UINT16, ::int16_t, short, Uint16, );
        HANDLE_PRIMITIVE(UINT32, ::int32_t, int, Uint32, );
        HANDLE_PRIMITIVE(UINT64, ::int64_t, long, Uint64, L);
#undef HANDLE_PRIMITIVE

      case schema::Type::FLOAT32:
        kind = FieldKind::PRIMITIVE;
        if (defaultBody.getFloat32() != 0) {
          int32_t mask;
          float value = defaultBody.getFloat32();
          static_assert(sizeof(mask) == sizeof(value), "bug");
          memcpy(&mask, &value, sizeof(mask));
          defaultMask = kj::str(mask);
        }
        break;

      case schema::Type::FLOAT64:
        kind = FieldKind::PRIMITIVE;
        if (defaultBody.getFloat64() != 0) {
          int64_t mask;
          double value = defaultBody.getFloat64();
          static_assert(sizeof(mask) == sizeof(value), "bug");
          memcpy(&mask, &value, sizeof(mask));
          defaultMask = kj::str(mask, "L");
        }
        break;

      case schema::Type::TEXT:
        kind = FieldKind::BLOB;
        if (defaultBody.hasText()) {
          defaultOffset = field.getDefaultValueSchemaOffset();
          defaultSize = defaultBody.getText().size();
        }
        break;
      case schema::Type::DATA:
        kind = FieldKind::BLOB;
        if (defaultBody.hasData()) {
          defaultOffset = field.getDefaultValueSchemaOffset();
          defaultSize = defaultBody.getData().size();
        }
        break;

      case schema::Type::ENUM:
        kind = FieldKind::PRIMITIVE;
        if (defaultBody.getEnum() != 0) {
          defaultMask = kj::str("(short)", defaultBody.getEnum());
        }
        break;

      case schema::Type::STRUCT:
        kind = FieldKind::STRUCT;
        if (defaultBody.hasStruct()) {
          defaultOffset = field.getDefaultValueSchemaOffset();
        }
        break;
      case schema::Type::LIST:
        kind = FieldKind::LIST;
        if (defaultBody.hasList()) {
          defaultOffset = field.getDefaultValueSchemaOffset();
        }
        break;
      case schema::Type::INTERFACE:
        kind = FieldKind::INTERFACE;
        break;
      case schema::Type::ANY_POINTER:
        kind = FieldKind::ANY_POINTER;
        if (defaultBody.hasAnyPointer()) {
          defaultOffset = field.getDefaultValueSchemaOffset();
        }
        break;
    }

    kj::String defaultMaskParam;
    if (defaultMask.size() > 0) {
      defaultMaskParam = kj::str(", ", defaultMask);
    }

    uint offset = slot.getOffset();

    auto structSchema = field.getContainingStruct();

    if (kind == FieldKind::PRIMITIVE) {
      return FieldText{
        kj::strTree(
          kj::mv(unionDiscrim.readerIsDef),
          spaces(indent),
          "  public ",
          readerType,
          " get",
          titleCase,
          "() {\n",
          unionDiscrim.check,
          (typeBody.which() == schema::Type::ENUM
             ? makeEnumGetter(
                 field.getType().asEnum(), offset, kj::str(defaultMaskParam), indent + 2)
             : (typeBody.which() == schema::Type::VOID
                  ? kj::strTree(spaces(indent), "    return Capnp.Void.VOID;\n")
                  : kj::strTree(
                      spaces(indent),
                      "    return _get",
                      toTitleCase(readerType),
                      "Field(",
                      offset,
                      defaultMaskParam,
                      ");\n"))),
          spaces(indent),
          "  }\n",
          "\n"),

        kj::strTree(
          kj::mv(unionDiscrim.builderIsDef),
          spaces(indent),
          "  public ",
          builderType,
          " get",
          titleCase,
          "() {\n",
          unionDiscrim.check,
          (typeBody.which() == schema::Type::ENUM
             ? makeEnumGetter(
                 field.getType().asEnum(), offset, kj::str(defaultMaskParam), indent + 2)
             : (typeBody.which() == schema::Type::VOID
                  ? kj::strTree(spaces(indent), "    return Capnp.Void.VOID;\n")
                  : kj::strTree(
                      spaces(indent),
                      "    return _get",
                      toTitleCase(builderType),
                      "Field(",
                      offset,
                      defaultMaskParam,
                      ");\n"))),
          spaces(indent),
          "  }\n",

          spaces(indent),
          "  public void set",
          titleCase,
          "(",
          readerType,
          " value) {\n",
          unionDiscrim.set,
          (typeBody.which() == schema::Type::ENUM
             ? kj::strTree(
                 spaces(indent),
                 "    _setShortField(",
                 offset,
                 ", "
                 "(short)((System.IConvertible)value).ToInt16(System.Globalization.CultureInfo."
                 "CurrentCulture)",
                 defaultMaskParam,
                 ");\n")
             : (typeBody.which() == schema::Type::VOID ? kj::strTree()
                                                       : kj::strTree(
                                                           spaces(indent),
                                                           "    _set",
                                                           toTitleCase(builderType),
                                                           "Field(",
                                                           offset,
                                                           ", value",
                                                           defaultMaskParam,
                                                           ");\n"))),
          spaces(indent),
          "  }\n",
          "\n")};

    } else if (kind == FieldKind::INTERFACE) {
      KJ_FAIL_REQUIRE("interfaces unimplemented");

    } else if (kind == FieldKind::ANY_POINTER) {

      auto factoryArg = makeFactoryArg(field.getType());

      return FieldText{
        kj::strTree(
          kj::mv(unionDiscrim.readerIsDef),
          spaces(indent),
          "  public bool has",
          titleCase,
          "() {\n",
          unionDiscrim.has,
          spaces(indent),
          "    return !_pointerFieldIsNull(",
          offset,
          ");\n",
          spaces(indent),
          "  }\n",

          spaces(indent),
          "  public ",
          readerType,
          " get",
          titleCase,
          "() {\n",
          unionDiscrim.check,
          spaces(indent),
          "    return _getPointerField(",
          factoryArg,
          ", ",
          offset,
          ");\n",
          spaces(indent),
          "  }\n"),

        kj::strTree(
          kj::mv(unionDiscrim.builderIsDef),
          spaces(indent),
          "  public bool has",
          titleCase,
          "() {\n",
          spaces(indent),
          "    return !_pointerFieldIsNull(",
          offset,
          ");\n",
          spaces(indent),
          "  }\n",

          spaces(indent),
          "  public ",
          builderType,
          " get",
          titleCase,
          "() {\n",
          unionDiscrim.check,
          spaces(indent),
          "    return _getPointerField(",
          factoryArg,
          ", ",
          offset,
          ");\n",
          spaces(indent),
          "  }\n",

          spaces(indent),
          "  public ",
          builderType,
          " init",
          titleCase,
          "() {\n",
          unionDiscrim.set,
          spaces(indent),
          "    return _initPointerField(",
          factoryArg,
          ", ",
          offset,
          ", 0);\n",
          spaces(indent),
          "  }\n",

          spaces(indent),
          "  public ",
          builderType,
          " init",
          titleCase,
          "(int size) {\n",
          unionDiscrim.set,
          spaces(indent),
          "    return _initPointerField(",
          factoryArg,
          ", ",
          offset,
          ", size);\n",
          spaces(indent),
          "  }\n",

          (field.getType().getBrandParameter() == nullptr ? kj::strTree()
                                                          : kj::strTree(
                                                              spaces(indent),
                                                              "  public <",
                                                              readerType,
                                                              "> void set",
                                                              titleCase,
                                                              "(Capnp.SetPointerBuilder<",
                                                              builderType,
                                                              ",",
                                                              readerType,
                                                              "> factory,",
                                                              readerType,
                                                              " value) {\n",
                                                              unionDiscrim.set,
                                                              spaces(indent),
                                                              "    _setPointerField(factory, ",
                                                              offset,
                                                              ", value);\n",
                                                              spaces(indent),
                                                              "  }\n")),

          "\n"),
      };

    } else if (kind == FieldKind::STRUCT) {
      uint64_t typeId = field.getContainingStruct().getProto().getId();
      kj::String defaultParams = defaultOffset == 0
        ? kj::str("null, 0")
        : kj::str("Schemas.B", kj::hex(typeId), ".b_", kj::hex(typeId), ", ", defaultOffset);

      auto typeParamVec = getTypeParameters(field.getContainingStruct());
      auto factoryArg = makeFactoryArg(field.getType());

      return FieldText{
        kj::strTree(
          kj::mv(unionDiscrim.readerIsDef),
          spaces(indent),
          "  public bool has",
          titleCase,
          "() {\n",
          spaces(indent),
          "    return !_pointerFieldIsNull(",
          offset,
          ");\n",
          spaces(indent),
          "  }\n",

          spaces(indent),
          "  public ",
          readerType,
          " get",
          titleCase,
          "() {\n",
          unionDiscrim.check,
          spaces(indent),
          "    return ",
          "_getPointerField(",
          factoryArg,
          ",",
          offset,
          ",",
          defaultParams,
          ");\n",
          spaces(indent),
          "  }\n",
          "\n"),

        kj::strTree(
          kj::mv(unionDiscrim.builderIsDef),
          spaces(indent),
          "  public ",
          builderType,
          " get",
          titleCase,
          "() {\n",
          unionDiscrim.check,
          spaces(indent),
          "    return ",
          "_getPointerField(",
          factoryArg,
          ", ",
          offset,
          ", ",
          defaultParams,
          ");\n",
          spaces(indent),
          "  }\n",

          (field.getType().asStruct().getProto().getIsGeneric()
             ? kj::strTree(
                 spaces(indent),
                 "  public ",
                 (typeParamVec.size() == 0
                    ? kj::strTree()
                    : kj::strTree(
                        "<",
                        kj::StringTree(
                          KJ_MAP(p, typeParamVec) { return kj::strTree(p, "_Reader"); }, ", "),
                        "> ")),
                 "void set",
                 titleCase,
                 "(Capnp.SetPointerBuilder<",
                 builderType,
                 ", ",
                 readerType,
                 "> factory, ",
                 readerType,
                 " value) {\n",
                 unionDiscrim.set,
                 spaces(indent),
                 "    _setPointerField(factory, ",
                 offset,
                 ", value);\n",
                 spaces(indent),
                 "  }\n")
             : kj::strTree(
                 spaces(indent),
                 "  public void set",
                 titleCase,
                 "(",
                 readerType,
                 " value) {\n",
                 unionDiscrim.set,
                 spaces(indent),
                 "    _setPointerField(",
                 factoryArg,
                 ",",
                 offset,
                 ", value);\n",
                 spaces(indent),
                 "  }\n")),

          spaces(indent),
          "  public ",
          builderType,
          " init",
          titleCase,
          "() {\n",
          unionDiscrim.set,
          spaces(indent),
          "    return ",
          "_initPointerField(",
          factoryArg,
          ",",
          offset,
          ", 0);\n",
          spaces(indent),
          "  }\n"),
      };

    } else if (kind == FieldKind::BLOB) {

      uint64_t typeId = field.getContainingStruct().getProto().getId();
      kj::String defaultParams = defaultOffset == 0
        ? kj::str("null, 0, 0")
        : kj::str("Schemas.B", kj::hex(typeId), ".b_", kj::hex(typeId), ".buffer, ", defaultOffset, ", ", defaultSize);

      kj::String blobKind =
        typeBody.which() == schema::Type::TEXT ? kj::str("Text") : kj::str("Data");
      kj::String setterInputType =
        typeBody.which() == schema::Type::TEXT ? kj::str("string") : kj::str("byte []");
      kj::String factory = kj::str("Capnp.", kj::str(blobKind), ".factory");

      return FieldText{
        kj::strTree(
          kj::mv(unionDiscrim.readerIsDef),
          spaces(indent),
          "  public bool has",
          titleCase,
          "() {\n",
          unionDiscrim.has,
          spaces(indent),
          "    return !_pointerFieldIsNull(",
          offset,
          ");\n",
          spaces(indent),
          "  }\n",

          spaces(indent),
          "  public ",
          readerType,
          " get",
          titleCase,
          "() {\n",
          spaces(indent),
          "    return _getPointerField(",
          factory,
          ", ",
          offset,
          ", ",
          defaultParams,
          ");\n",
          spaces(indent),
          "  }\n",
          "\n"),

        kj::strTree(
          kj::mv(unionDiscrim.builderIsDef),
          spaces(indent),
          "  public bool has",
          titleCase,
          "() {\n",
          unionDiscrim.has,
          spaces(indent),
          "    return !_pointerFieldIsNull(",
          offset,
          ");\n",
          spaces(indent),
          "  }\n",
          spaces(indent),
          "  public ",
          builderType,
          " get",
          titleCase,
          "() {\n",
          spaces(indent),
          "    return _getPointerField(",
          factory,
          ", ",
          offset,
          ", ",
          defaultParams,
          ");\n",
          spaces(indent),
          "  }\n",
          spaces(indent),
          "  public void set",
          titleCase,
          "(",
          readerType,
          " value) {\n",
          unionDiscrim.set,
          spaces(indent),
          "    _setPointerField(",
          factory,
          ", ",
          offset,
          ", value);\n",
          spaces(indent),
          "  }\n",
          spaces(indent),
          "  public void set",
          titleCase,
          "(",
          setterInputType,
          " value) {\n",
          unionDiscrim.set,
          spaces(indent),
          "    _setPointerField(",
          factory,
          ", ",
          offset,
          ", new ",
          readerType,
          "(value));\n",
          spaces(indent),
          "  }\n",

          spaces(indent),
          "  public ",
          builderType,
          " init",
          titleCase,
          "(int size) {\n",
          unionDiscrim.set,
          spaces(indent),
          "    return _initPointerField(",
          factory,
          ", ",
          offset,
          ", size);\n",
          spaces(indent),
          "  }\n"),
      };
    } else if (kind == FieldKind::LIST) {

      uint64_t typeId = field.getContainingStruct().getProto().getId();
      kj::String defaultParams = defaultOffset == 0
        ? kj::str("null, 0")
        : kj::str("Schemas.B", kj::hex(typeId), ".b_", kj::hex(typeId), ", ", defaultOffset);

      auto typeParamVec = getTypeParameters(field.getContainingStruct());
      kj::String listFactory = makeFactoryArg(field.getType());

      bool isGeneric = false;
      {
        auto type = field.getType();
        while (type.isList()) {
          type = type.asList().getElementType();
        }

        if (type.isStruct()) {
          isGeneric = type.asStruct().getProto().getIsGeneric();
        }
      }

      return FieldText{
        kj::strTree(
          kj::mv(unionDiscrim.readerIsDef),
          spaces(indent),
          "  public bool has",
          titleCase,
          "() {\n",
          spaces(indent),
          "    return !_pointerFieldIsNull(",
          offset,
          ");\n",
          spaces(indent),
          "  }\n",

          (isGeneric
             ? kj::strTree(
                 spaces(indent),
                 "  public ",
                 (typeParamVec.size() == 0
                    ? kj::strTree()
                    : kj::strTree(
                        "<",
                        kj::StringTree(
                          KJ_MAP(p, typeParamVec) { return kj::strTree(p, "_Builder"); }, ", "),
                        "> ")),
                 readerType,
                 " get",
                 titleCase,
                 "( Capnp.ListFactory<",
                 builderType,
                 ", ",
                 readerType,
                 "> factory) {\n",
                 spaces(indent),
                 "    return _getPointerField(factory,",
                 offset,
                 ");\n",
                 spaces(indent),
                 "  }\n")
             : kj::strTree(
                 spaces(indent),
                 "  public ",
                 readerType,
                 " get",
                 titleCase,
                 "() {\n",
                 spaces(indent),
                 "    return _getPointerField(",
                 listFactory,
                 ", ",
                 offset,
                 ", ",
                 defaultParams,
                 ");\n",
                 spaces(indent),
                 "  }\n")),
          "\n"),

        kj::strTree(
          kj::mv(unionDiscrim.builderIsDef),
          spaces(indent),
          "  public bool has",
          titleCase,
          "() {\n",
          spaces(indent),
          "    return !_pointerFieldIsNull(",
          offset,
          ");\n",
          spaces(indent),
          "  }\n",

          (isGeneric
             ? kj::strTree(
                 spaces(indent),
                 "  public ",
                 (typeParamVec.size() == 0
                    ? kj::strTree()
                    : kj::strTree(
                        "<",
                        kj::StringTree(
                          KJ_MAP(p, typeParamVec) { return kj::strTree(p, "_Reader"); }, ", "),
                        "> ")),
                 builderType,
                 " get",
                 titleCase,
                 "( Capnp.ListFactory<",
                 builderType,
                 ", ",
                 readerType,
                 "> factory) {\n",
                 spaces(indent),
                 "    return _getPointerField(factory,",
                 offset,
                 ");\n",
                 spaces(indent),
                 "  }\n")
             : kj::strTree(
                 spaces(indent),
                 "  public ",
                 builderType,
                 " get",
                 titleCase,
                 "() {\n",
                 spaces(indent),
                 "    return _getPointerField(",
                 listFactory,
                 ", ",
                 offset,
                 ", ",
                 defaultParams,
                 ");\n",
                 spaces(indent),
                 "  }\n")),

          (isGeneric
             ? kj::strTree(
                 spaces(indent),
                 "  public ",
                 (typeParamVec.size() == 0
                    ? kj::strTree()
                    : kj::strTree(
                        "<",
                        kj::StringTree(
                          KJ_MAP(p, typeParamVec) { return kj::strTree(p, "_Reader"); }, ", "),
                        "> ")),
                 "void set",
                 titleCase,
                 "(Capnp.SetPointerBuilder<",
                 builderType,
                 ", ",
                 readerType,
                 "> factory, ",
                 readerType,
                 " value) {\n",
                 unionDiscrim.set,
                 spaces(indent),
                 "    _setPointerField(factory, ",
                 offset,
                 ", value);\n",
                 spaces(indent),
                 "  }\n")
             : kj::strTree(
                 spaces(indent),
                 "  public void set",
                 titleCase,
                 "(",
                 readerType,
                 " value) {\n",
                 unionDiscrim.set,
                 spaces(indent),
                 "    _setPointerField(",
                 listFactory,
                 ", ",
                 offset,
                 ", value);\n",
                 spaces(indent),
                 "  }\n")),

          (isGeneric
             ? kj::strTree(
                 spaces(indent),
                 "  public ",
                 (typeParamVec.size() == 0
                    ? kj::strTree()
                    : kj::strTree(
                        "<",
                        kj::StringTree(
                          KJ_MAP(p, typeParamVec) { return kj::strTree(p, "_Reader"); }, ", "),
                        "> ")),
                 builderType,
                 " init",
                 titleCase,
                 "( Capnp.ListFactory<",
                 builderType,
                 ", ",
                 readerType,
                 "> factory, int size) {\n",
                 unionDiscrim.set,
                 spaces(indent),
                 "    return _initPointerField(factory, ",
                 offset,
                 ", size);\n",
                 spaces(indent),
                 "  }\n")
             : kj::strTree(
                 spaces(indent),
                 "  public ",
                 builderType,
                 " init",
                 titleCase,
                 "(int size) {\n",
                 unionDiscrim.set,
                 spaces(indent),
                 "    return _initPointerField(",
                 listFactory,
                 ", ",
                 offset,
                 ", size);\n",
                 spaces(indent),
                 "  }\n"))),

      };
    } else {
      KJ_UNREACHABLE;
    }
  }

  // -----------------------------------------------------------------

  struct StructText {
    kj::StringTree outerTypeDef;
    kj::StringTree readerBuilderDefs;
    kj::StringTree inlineMethodDefs;
  };

  kj::StringTree makeWhich(StructSchema schema, int indent) {
    if (schema.getProto().getStruct().getDiscriminantCount() == 0) {
      return kj::strTree();
    } else {
      auto fields = schema.getUnionFields();
      return kj::strTree(
        spaces(indent),
        "public Which which() {\n",
        spaces(indent + 1),
        "switch(_getShortField(",
        schema.getProto().getStruct().getDiscriminantOffset(),
        ")) {\n",
        KJ_MAP(f, fields) {
          return kj::strTree(
            spaces(indent + 2),
            "case ",
            f.getProto().getDiscriminantValue(),
            " : return ",
            "Which.",
            toUpperCase(f.getProto().getName()),
            ";\n");
        },
        spaces(indent + 2),
        "default: return Which._NOT_IN_SCHEMA;\n",
        spaces(indent + 1),
        "}\n",
        spaces(indent),
        "}\n");
    }
  }

  StructText makeStructText(
    kj::StringPtr scope,
    kj::StringPtr name,
    StructSchema schema,
    kj::Array<kj::StringTree> nestedTypeDecls,
    int indent) {
    auto proto = schema.getProto();
    auto fullName = kj::str(scope, name);
    auto subScope = kj::str(fullName, ".");
    auto fieldTexts = KJ_MAP(f, schema.getFields()) {
      return makeFieldText(subScope, f, indent + 1);
    };

    auto structNode = proto.getStruct();
    uint discrimOffset = structNode.getDiscriminantOffset();
    structNode.getPointerCount();

    auto typeParamVec = getTypeParameters(schema);
    bool hasTypeParams = typeParamVec.size() > 0;

    kj::StringTree readerTypeParamsTree;
    kj::StringTree builderTypeParamsTree;
    kj::StringTree factoryTypeParamsTree;
    if (hasTypeParams) {
      builderTypeParamsTree = kj::strTree(
        "<",
        kj::StringTree(KJ_MAP(p, typeParamVec) { return kj::strTree(p, "_Builder"); }, ", "),
        ">");
      readerTypeParamsTree = kj::strTree(
        "<",
        kj::StringTree(KJ_MAP(p, typeParamVec) { return kj::strTree(p, "_Reader"); }, ", "),
        ">");

      factoryTypeParamsTree = kj::strTree(
        "<",
        kj::StringTree(
          KJ_MAP(p, typeParamVec) { return kj::strTree(p, "_Builder, ", p, "_Reader"); }, ", "),
        ">");
    }
    kj::String readerTypeParams = readerTypeParamsTree.flatten();
    kj::String builderTypeParams = builderTypeParamsTree.flatten();
    kj::String factoryTypeParams = factoryTypeParamsTree.flatten();

    kj::StringTree factoryArgs = kj::StringTree(
      KJ_MAP(p, typeParamVec) {
        return kj::strTree("Capnp.PointerFactory<", p, "_Builder, ", p, "_Reader> ", p, "_Factory");
      },
      ", ");

    kj::StringTree factoryMembers = kj::strTree(KJ_MAP(p, typeParamVec) {
      return kj::strTree(
        spaces(indent),
        "    readonly Capnp.PointerFactory<",
        p,
        "_Builder, ",
        p,
        "_Reader> ",
        p,
        "_Factory;\n");
    });

    return StructText{
      kj::strTree(
        spaces(indent),
        "public static class ",
        name,
        " {\n",
        kj::strTree(
          spaces(indent),
          "  public static readonly Capnp.StructSize STRUCT_SIZE =",
          " new Capnp.StructSize((short)",
          structNode.getDataWordCount(),
          ",(short)",
          structNode.getPointerCount(),
          ");\n"),

        spaces(indent),
        "  public class Factory",
        factoryTypeParams,
        " : Capnp.StructFactory<Builder",
        builderTypeParams,
        ", Reader",
        readerTypeParams,
        "> {\n",
        factoryMembers.flatten(),
        spaces(indent),
        "    public Factory(",
        factoryArgs.flatten(),
        ") {\n",
        KJ_MAP(p, typeParamVec) {
          return kj::strTree(spaces(indent), "      this.", p, "_Factory = ", p, "_Factory;\n");
        },
        spaces(indent),
        "    }\n",

        spaces(indent),
        "    public override Reader",
        readerTypeParams,
        " constructReader(Capnp.SegmentReader segment, int data,",
        "int pointers, int dataSize, short pointerCount, int nestingLimit) {\n",
        spaces(indent),
        "      return new Reader",
        readerTypeParams,
        "(",
        KJ_MAP(p, typeParamVec) { return kj::strTree(p, "_Factory, "); },
        "segment,data,pointers,dataSize,pointerCount,nestingLimit);\n",
        spaces(indent),
        "    }\n",
        spaces(indent),
        "    public override Builder",
        builderTypeParams,
        " constructBuilder(Capnp.SegmentBuilder segment, int data,",
        "int pointers, int dataSize, short pointerCount) {\n",
        spaces(indent),
        "      return new Builder",
        builderTypeParams,
        "(",
        KJ_MAP(p, typeParamVec) { return kj::strTree(p, "_Factory, "); },
        "segment, data, pointers, dataSize, pointerCount);\n",
        spaces(indent),
        "    }\n",
        spaces(indent),
        "    public override Capnp.StructSize structSize() {\n",
        spaces(indent),
        "      return ",
        fullName,
        ".STRUCT_SIZE;\n",
        spaces(indent),
        "    }\n",
        spaces(indent),
        "    public override Reader",
        readerTypeParams,
        " asReader(Builder",
        builderTypeParams,
        " builder) {\n",
        spaces(indent),
        "      return builder.asReader(",
        (hasTypeParams ? kj::strTree("this") : kj::strTree()),
        ");\n",
        spaces(indent),
        "    }\n",

        spaces(indent),
        "  }\n",
        (hasTypeParams
           ? kj::strTree(
               spaces(indent),
               "  public static readonly ",
               factoryTypeParams,
               "Factory",
               factoryTypeParams,
               "\n",
               spaces(indent),
               "    newFactory(",
               factoryArgs.flatten(),
               "){\n",
               spaces(indent),
               "   return new Factory",
               factoryTypeParams,
               "(",
               kj::StringTree(KJ_MAP(p, typeParamVec) { return kj::strTree(p, "_Factory"); }, ", "),
               ");\n",
               spaces(indent),
               "  }\n")
           : kj::strTree(
               spaces(indent),
               "  public static readonly Factory factory = new Factory();\n",
               spaces(indent),
               "  public static readonly Capnp.StructList.Factory<Builder,Reader> listFactory =\n",
               spaces(indent),
               "    new Capnp.StructList.Factory<Builder, Reader>(factory);\n")),

        kj::strTree(
          spaces(indent + 1),
          "public class Builder",
          builderTypeParams,
          " : Capnp.StructBuilder {\n",
          kj::strTree(KJ_MAP(p, typeParamVec) {
            return kj::strTree(
              spaces(indent),
              "    readonly Capnp.PointerFactory<",
              p,
              "_Builder, ?> ",
              p,
              "_Factory;\n");
          }),
          spaces(indent + 1),
          "  public Builder(",
          KJ_MAP(p, typeParamVec) {
            return kj::strTree("Capnp.PointerFactory<", p, "_Builder, ?> ", p, "_Factory,");
          },
          "Capnp.SegmentBuilder segment, int data, int pointers,",
          "int dataSize, short pointerCount) : base(segment, data, pointers, dataSize, "
          "pointerCount) {\n",
          KJ_MAP(p, typeParamVec) {
            return kj::strTree(spaces(indent), "      this.", p, "_Factory = ", p, "_Factory;\n");
          },
          spaces(indent + 1),
          "  }\n",
          makeWhich(schema, indent + 2),
          spaces(indent + 1),
          "  public ",
          readerTypeParams,
          "Reader",
          readerTypeParams,
          " asReader(",
          (!hasTypeParams ? kj::strTree()
                          : kj::strTree(name, ".Factory", factoryTypeParams, " factory")),
          ") {\n",
          spaces(indent + 1),
          "    return new Reader",
          readerTypeParams,
          "(",
          KJ_MAP(p, typeParamVec) { return kj::strTree("factory.", p, "_Factory, "); },
          "segment, data, pointers, dataSize, pointerCount, 0x7fffffff);\n",
          spaces(indent + 1),
          "  }\n",
          KJ_MAP(f, fieldTexts) { return kj::mv(f.builderMethodDecls); },
          spaces(indent + 1),
          "}\n",
          "\n"),

        kj::strTree(
          spaces(indent + 1),
          "public class Reader",
          readerTypeParams,
          " : Capnp.StructReader {\n",
          KJ_MAP(p, typeParamVec) {
            return kj::strTree(
              spaces(indent), "    Capnp.PointerFactory<?,", p, "_Reader> ", p, "_Factory;\n");
          },
          spaces(indent + 1),
          "  public Reader(",
          KJ_MAP(p, typeParamVec) {
            return kj::strTree("Capnp.PointerFactory<?,", p, "_Reader> ", p, "_Factory,");
          },
          "Capnp.SegmentReader segment, int data, int pointers,",
          "int dataSize, short pointerCount, int nestingLimit) : base(segment, data, pointers, "
          "dataSize, pointerCount, nestingLimit) {\n",
          KJ_MAP(p, typeParamVec) {
            return kj::strTree(spaces(indent), "      this.", p, "_Factory = ", p, "_Factory;\n");
          },
          spaces(indent + 1),
          "  }\n",
          "\n",
          makeWhich(schema, indent + 2),
          KJ_MAP(f, fieldTexts) { return kj::mv(f.readerMethodDecls); },
          spaces(indent + 1),
          "}\n"
          "\n"),

        structNode.getDiscriminantCount() == 0
          ? kj::strTree()
          : kj::strTree(
              spaces(indent),
              "  public enum Which {\n",
              KJ_MAP(f, structNode.getFields()) {
                if (hasDiscriminantValue(f)) {
                  return kj::strTree(spaces(indent), "    ", toUpperCase(f.getName()), ",\n");
                } else {
                  return kj::strTree();
                }
              },
              spaces(indent),
              "    _NOT_IN_SCHEMA,\n",
              spaces(indent),
              "  }\n"),
        KJ_MAP(n, nestedTypeDecls) { return kj::mv(n); },
        spaces(indent),
        "}\n"
        "\n",
        "\n"),

      kj::strTree(),
      kj::strTree()};
  }

  // -----------------------------------------------------------------

  struct ConstText {
    bool needsSchema;
    kj::StringTree decl;
  };

  ConstText makeConstText(kj::StringPtr scope, kj::StringPtr name, ConstSchema schema, int indent) {
    auto proto = schema.getProto();
    auto constProto = proto.getConst();
    auto type = schema.getType();
    auto typeName_ = typeName(type, kj::str("Reader")).flatten();
    auto upperCase = toUpperCase(name);

    switch (type.which()) {
      case schema::Value::VOID:
      case schema::Value::BOOL:
      case schema::Value::INT8:
      case schema::Value::INT16:
      case schema::Value::INT32:
      case schema::Value::INT64:
      case schema::Value::UINT8:
      case schema::Value::UINT16:
      case schema::Value::UINT32:
      case schema::Value::UINT64:
      case schema::Value::FLOAT32:
      case schema::Value::FLOAT64:
      case schema::Value::ENUM:
        return ConstText{false,
                         kj::strTree(
                           spaces(indent),
                           "public static readonly ",
                           typeName_,
                           ' ',
                           upperCase,
                           " = ",
                           literalValue(constProto.getType(), constProto.getValue()),
                           ";\n")};

      case schema::Value::TEXT: {
        return ConstText{true,
                         kj::strTree(
                           spaces(indent),
                           "public static readonly Capnp.Text.Reader ",
                           upperCase,
                           " = new Capnp.Text.Reader(Schemas.B",
                           kj::hex(proto.getId()),
                           ".b_",
                           kj::hex(proto.getId()),
                           ".buffer, ",
                           schema.getValueSchemaOffset(),
                           ", ",
                           constProto.getValue().getText().size(),
                           ");\n")};
      }

      case schema::Value::DATA: {
        return ConstText{true,
                         kj::strTree(
                           spaces(indent),
                           "public static readonly Capnp.Data.Reader ",
                           upperCase,
                           " = new Capnp.Data.Reader(Schemas.B",
                           kj::hex(proto.getId()),
                           ".b_",
                           kj::hex(proto.getId()),
                           ".buffer, ",
                           schema.getValueSchemaOffset(),
                           ", ",
                           constProto.getValue().getData().size(),
                           ");\n")};
      }

      case schema::Value::STRUCT: {
        return ConstText{true,
                         kj::strTree(
                           spaces(indent),
                           "public static readonly ",
                           typeName_,
                           " ",
                           upperCase,
                           " =\n",
                           spaces(indent),
                           "  ",
                           "new Capnp.AnyPointer.Reader(Schemas.B",
                           kj::hex(proto.getId()),
                           ".b_",
                           kj::hex(proto.getId()),
                           ",",
                           schema.getValueSchemaOffset(),
                           ",0x7fffffff).getAs(",
                           makeFactoryArg(type),
                           ");\n")};
      }

      case schema::Value::LIST: {
        return ConstText{true,
                         kj::strTree(
                           spaces(indent),
                           "public static readonly ",
                           typeName_,
                           ' ',
                           upperCase,
                           " =\n",
                           spaces(indent),
                           " (",
                           "new Capnp.AnyPointer.Reader(Schemas.B",
                           kj::hex(proto.getId()),
                           ".b_",
                           kj::hex(proto.getId()),
                           ",",
                           schema.getValueSchemaOffset(),
                           ",0x7fffffff).getAs(",
                           makeFactoryArg(type),
                           "));\n")};
      }

      case schema::Value::ANY_POINTER:
      case schema::Value::INTERFACE:
        return ConstText{false, kj::strTree()};
    }

    KJ_UNREACHABLE;
  }

  // -----------------------------------------------------------------

  struct NodeText {
    kj::StringTree outerTypeDef;
    kj::StringTree readerBuilderDefs;
    kj::StringTree inlineMethodDefs;
    kj::StringTree capnpSchemaDefs;
    kj::StringTree capnpPrivateDefs;
    kj::StringTree sourceFileDefs;
  };

  struct NodeTextNoSchema {
    kj::StringTree outerTypeDef;
    kj::StringTree readerBuilderDefs;
    kj::StringTree inlineMethodDefs;
    kj::StringTree capnpPrivateDefs;
    kj::StringTree sourceFileDefs;
  };

  NodeText makeNodeText(kj::StringPtr scope, kj::StringPtr name, Schema schema, int indent) {
    auto proto = schema.getProto();
    auto fullName = kj::str(scope, name);
    auto subScope = kj::str(fullName, ".");
    auto hexId = kj::hex(proto.getId());

    // Compute nested nodes, including groups.
    kj::Vector<NodeText> nestedTexts(proto.getNestedNodes().size());
    for (auto nested : proto.getNestedNodes()) {
      nestedTexts.add(makeNodeText(
        subScope, nested.getName(), schemaLoader.getUnbound(nested.getId()), indent + 1));
    };

    if (proto.isStruct()) {
      for (auto field : proto.getStruct().getFields()) {
        if (field.isGroup()) {
          nestedTexts.add(makeNodeText(
            subScope,
            toTitleCase(field.getName()),
            schemaLoader.getUnbound(field.getGroup().getTypeId()),
            indent + 1));
        }
      }
    } else if (proto.isInterface()) {
      KJ_FAIL_REQUIRE("interfaces not implemented");
    }

    // Convert the encoded schema to a literal byte array.
    kj::ArrayPtr<const word> rawSchema = schema.asUncheckedMessage();
    auto schemaLiteral = kj::StringTree(
      KJ_MAP(w, rawSchema) {
        const byte *bytes = reinterpret_cast<const byte *>(&w);

        return kj::strTree(
          "\"",
          KJ_MAP(i, kj::range<uint>(0, sizeof(word))) {
            switch (bytes[i]) {
              case 0x0a:
                return kj::strTree("\\n");
              case 0x0d:
                return kj::strTree("\\r");
              case 0x22:
                return kj::strTree("\\\"");
              case 0x5c:
                return kj::strTree("\\\\");
              default:
                auto text = kj::hex(bytes[i]);
                return kj::strTree("\\u", kj::repeat('0', 4 - text.size()), text);
            }
          },
          "\" +");
      },
      "\n   ");

    std::set<uint64_t> deps;
    enumerateDeps(proto, deps);

    kj::Array<uint> membersByName;
    kj::Array<uint> membersByDiscrim;
    switch (proto.which()) {
      case schema::Node::STRUCT: {
        auto structSchema = schema.asStruct();
        membersByName = makeMembersByName(structSchema.getFields());
        auto builder = kj::heapArrayBuilder<uint>(structSchema.getFields().size());
        for (auto field : structSchema.getUnionFields()) {
          builder.add(field.getIndex());
        }
        for (auto field : structSchema.getNonUnionFields()) {
          builder.add(field.getIndex());
        }
        membersByDiscrim = builder.finish();
        break;
      }
      case schema::Node::ENUM:
        membersByName = makeMembersByName(schema.asEnum().getEnumerants());
        break;
      case schema::Node::INTERFACE:
        membersByName = makeMembersByName(schema.asInterface().getMethods());
        break;
      default:
        break;
    }

    // Java limits method code size to 64KB. Maybe we should use class.getResource()?
    auto schemaDef = kj::strTree(
      "public static class B",
      hexId,
      " {\n",
      "public static readonly Capnp.SegmentReader b_",
      hexId,
      " =\n",
      "   Capnp.GeneratedClassSupport.decodeRawBytes(\n",
      "   ",
      kj::mv(schemaLiteral),
      " \"\"",
      ");\n",
      "}\n");
    /*
            deps.size() == 0 ? kj::strTree() : kj::strTree(
                "static const ::capnp::_::RawSchema* const d_", hexId, "[] = {\n",
                KJ_MAP(depId, deps) {
                  return kj::strTree("  &s_", kj::hex(depId), ",\n");
                },
                "};\n"),
            membersByName.size() == 0 ? kj::strTree() : kj::strTree(
                "static const uint16_t m_", hexId, "[] = {",
                kj::StringTree(KJ_MAP(index, membersByName) { return kj::strTree(index); }, ", "),
                "};\n"),
            membersByDiscrim.size() == 0 ? kj::strTree() : kj::strTree(
                "static const uint16_t i_", hexId, "[] = {",
                kj::StringTree(KJ_MAP(index, membersByDiscrim) { return kj::strTree(index); }, ",
       "),
                "};\n"),
            "const ::capnp::_::RawSchema s_", hexId, " = {\n"
            "  0x", hexId, ", b_", hexId, ".words, ", rawSchema.size(), ", ",
            deps.size() == 0 ? kj::strTree("nullptr") : kj::strTree("d_", hexId), ", ",
            membersByName.size() == 0 ? kj::strTree("nullptr") : kj::strTree("m_", hexId), ",\n",
            "  ", deps.size(), ", ", membersByName.size(), ", ",
            membersByDiscrim.size() == 0 ? kj::strTree("nullptr") : kj::strTree("i_", hexId),
            ", nullptr, nullptr\n"
            "};\n");*/

    NodeTextNoSchema top = makeNodeTextWithoutNested(
      scope, name, schema, KJ_MAP(n, nestedTexts) { return kj::mv(n.outerTypeDef); }, indent);

    return NodeText{
      kj::strTree(
        kj::mv(top.outerTypeDef), KJ_MAP(n, nestedTexts) { return kj::mv(n.outerTypeDef); }),

      kj::strTree(
        kj::mv(top.readerBuilderDefs),
        KJ_MAP(n, nestedTexts) { return kj::mv(n.readerBuilderDefs); }),

      kj::strTree(
        kj::mv(top.inlineMethodDefs),
        KJ_MAP(n, nestedTexts) { return kj::mv(n.inlineMethodDefs); }),

      kj::strTree(kj::mv(schemaDef), KJ_MAP(n, nestedTexts) { return kj::mv(n.capnpSchemaDefs); }),

      kj::strTree(
        kj::mv(top.capnpPrivateDefs),
        KJ_MAP(n, nestedTexts) { return kj::mv(n.capnpPrivateDefs); }),

      kj::strTree(
        kj::mv(top.sourceFileDefs), KJ_MAP(n, nestedTexts) { return kj::mv(n.sourceFileDefs); }),
    };
  }

  NodeTextNoSchema makeNodeTextWithoutNested(
    kj::StringPtr scope,
    kj::StringPtr name,
    Schema schema,
    kj::Array<kj::StringTree> nestedTypeDecls,
    int indent) {
    auto proto = schema.getProto();
    auto fullName = kj::str(scope, name);
    auto hexId = kj::hex(proto.getId());

    switch (proto.which()) {
      case schema::Node::FILE:
        KJ_FAIL_REQUIRE("This method shouldn't be called on file nodes.");

      case schema::Node::STRUCT: {
        StructText structText =
          makeStructText(scope, name, schema.asStruct(), kj::mv(nestedTypeDecls), indent);
        auto structNode = proto.getStruct();

        return NodeTextNoSchema{
          kj::mv(structText.outerTypeDef),
          kj::mv(structText.readerBuilderDefs),
          kj::mv(structText.inlineMethodDefs),

          kj::strTree(),
          kj::strTree(),
        };
      }

      case schema::Node::ENUM: {
        auto enumerants = schema.asEnum().getEnumerants();

        return NodeTextNoSchema{
          kj::strTree(
            spaces(indent),
            "public enum ",
            name,
            " {\n",
            KJ_MAP(e, enumerants) {
              return kj::strTree(spaces(indent), "  ", toUpperCase(e.getProto().getName()), ",\n");
            },
            spaces(indent),
            "  _NOT_IN_SCHEMA,\n",
            spaces(indent),
            "}\n"
            "\n"),

          kj::strTree(),
          kj::strTree(),
          kj::strTree(),
        };
      }

      case schema::Node::INTERFACE: {
        hasInterfaces = true;
        KJ_FAIL_REQUIRE("unimplemented");
      }

      case schema::Node::CONST: {
        auto constText = makeConstText(scope, name, schema.asConst(), indent);

        return NodeTextNoSchema{
          kj::strTree(kj::mv(constText.decl)),
          kj::strTree(),
          kj::strTree(),
          kj::strTree(),
          kj::strTree(),
        };
      }

      case schema::Node::ANNOTATION: {
        return NodeTextNoSchema{
          kj::strTree(), kj::strTree(), kj::strTree(), kj::strTree(), kj::strTree(),
        };
      }
    }

    KJ_UNREACHABLE;
  }

  // -----------------------------------------------------------------

  struct FileText {
    kj::String filename;
    kj::StringTree source;
  };

  FileText makeFileText(
    Schema schema, schema::CodeGeneratorRequest::RequestedFile::Reader request) {
    usedImports.clear();

    auto node = schema.getProto();
    auto displayName = node.getDisplayName();

    auto packageName = getCsNamespace(node);

    if (packageName.size() == 0) {
      context.exitError(kj::str(displayName, ": no CS package name found. See cs.capnp."));
    }

    auto nodeTexts = KJ_MAP(nested, node.getNestedNodes()) {
      return makeNodeText("", nested.getName(), schemaLoader.getUnbound(nested.getId()), 1);
    };

    kj::String separator = kj::str("// ", kj::repeat('=', 87), "\n");

    kj::Vector<kj::StringPtr> includes;
    for (auto import : request.getImports()) {
      if (usedImports.count(import.getId()) > 0) {
        includes.add(import.getName());
      }
    }

    kj::StringTree sourceDefs =
      kj::strTree(KJ_MAP(n, nodeTexts) { return kj::mv(n.sourceFileDefs); });

    return FileText{kj::str(baseName(displayName)),
                    kj::strTree(
                      "// Generated by Cap'n Proto compiler, DO NOT EDIT\n"
                      "// source: ",
                      baseName(displayName),
                      "\n\n",
                      "namespace ",
                      packageName,
                      " {\n\n",
                      KJ_MAP(n, nodeTexts) { return kj::mv(n.outerTypeDef); },
                      "\n",
                      "namespace Schemas {\n",
                      KJ_MAP(n, nodeTexts) { return kj::mv(n.capnpSchemaDefs); },
                      "}\n",
                      "}\n",
                      "\n")};
  }

  // -----------------------------------------------------------------

  void makeDirectory(kj::StringPtr path) {
    KJ_IF_MAYBE(slashpos, path.findLast('/')) {
      // Make the parent dir.
      makeDirectory(kj::str(path.slice(0, *slashpos)));
    }

    if (mkdir(path.cStr(), 0777) < 0) {
      int error = errno;
      if (error != EEXIST) {
        KJ_FAIL_SYSCALL("mkdir(path)", error, path);
      }
    }
  }

  void writeFile(kj::StringPtr filename, const kj::StringTree &text) {
    if (!filename.startsWith("/")) {
      KJ_IF_MAYBE(slashpos, filename.findLast('/')) {
        // Make the parent dir.
        makeDirectory(kj::str(filename.slice(0, *slashpos)));
      }
    }

    int fd;
    KJ_SYSCALL(fd = open(filename.cStr(), O_CREAT | O_WRONLY | O_TRUNC, 0666), filename);
    kj::FdOutputStream out((kj::AutoCloseFd(fd)));

    text.visit([&](kj::ArrayPtr<const char> text) { out.write(text.begin(), text.size()); });
  }

  kj::MainBuilder::Validity run() {
    ReaderOptions options;
    options.traversalLimitInWords = 1 << 30;  // Don't limit.
    StreamFdMessageReader reader(STDIN_FILENO, options);
    auto request = reader.getRoot<schema::CodeGeneratorRequest>();

    for (auto node : request.getNodes()) {
      schemaLoader.load(node);
    }

    kj::FdOutputStream rawOut(STDOUT_FILENO);
    kj::BufferedOutputStreamWrapper out(rawOut);

    for (auto requestedFile : request.getRequestedFiles()) {
      auto schema = schemaLoader.get(requestedFile.getId());

      auto filename = requestedFile.getFilename();
      size_t stemstart = 0;
      size_t stemend = filename.size();
      KJ_IF_MAYBE(slashpos, filename.findLast('/')) { stemstart = *slashpos + 1; }
      KJ_IF_MAYBE(dotpos, filename.findLast('.')) { stemend = *dotpos; }

      auto fileText = makeFileText(schema, requestedFile);

      writeFile(
        kj::str(filename.slice(0, stemstart), fileText.filename, ".cs"), fileText.source);
    }

    return true;
  }
};

}  // namespace
}  // namespace capnp

KJ_MAIN(capnp::CapnpcJavaMain);
