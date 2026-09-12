#include <clang/AST/ASTConsumer.h>
#include <clang/AST/RecursiveASTVisitor.h>
#include <clang/ASTMatchers/ASTMatchFinder.h>
#include <clang/ASTMatchers/ASTMatchers.h>
#include <clang/Frontend/CompilerInstance.h>
#include <clang/Frontend/FrontendAction.h>
#include <clang/Frontend/FrontendActions.h>
#include <clang/Tooling/CommonOptionsParser.h>
#include <clang/Tooling/Tooling.h>
#include <libgen.h>
#include <llvm/Support/CommandLine.h>
#include <unistd.h>

#include <algorithm>
#include <cctype>
#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <memory>
#include <sstream>
#include <stdexcept>
#include <string>

#include "bzl/inliner/build-rule.pb.h"
#include "c8/c8-log.h"
#include "c8/string.h"
#include "third_party/murmurhash/constexpr_murmur3.h"

using namespace clang;
using namespace llvm;
using namespace clang::ast_matchers;
using namespace clang::tooling;
using inliner::BuildRule;
using std::string;
using std::vector;
using namespace google::protobuf;

static constexpr char RULES1_DEP[] = "//bzl/inliner:rules";
static constexpr char RULES2_DEP[] = "//bzl/inliner:rules2";

DeclarationMatcher matcher = eachOf(
  // Inliner v2.
  functionDecl(
    matchesName("inlinerRule"),
    isDefinition(),
    hasDescendant(compoundStmt(hasDescendant(compoundStmt(forEachDescendant(cxxOperatorCallExpr(
      hasOverloadedOperatorName("="),
      eachOf(
        forEach(integerLiteral().bind("rhs")),
        forEachDescendant(
          materializeTemporaryExpr(has(cxxConstructExpr(has(stringLiteral().bind("rhs")))))),
        forEachDescendant(initListExpr().bind("rhs")),
        forEach(initListExpr().bind("rhs")),
        forEach(cxxBoolLiteral().bind("rhs"))),
      hasDescendant(declRefExpr(to(varDecl())).bind("lhs")))))))))
    .bind("build_func"),
  // Inliner v2 with no attribute assignments.
  functionDecl(
    matchesName("inlinerRule"),
    isDefinition(),
    unless(
      hasDescendant(compoundStmt(hasDescendant(compoundStmt(forEachDescendant(cxxOperatorCallExpr(
        hasOverloadedOperatorName("="),
        eachOf(
          forEach(integerLiteral().bind("rhs")),
          forEachDescendant(
            materializeTemporaryExpr(has(cxxConstructExpr(has(stringLiteral().bind("rhs")))))),
          forEachDescendant(initListExpr().bind("rhs")),
          forEach(initListExpr().bind("rhs")),
          forEach(cxxBoolLiteral().bind("rhs"))),
        hasDescendant(declRefExpr(to(varDecl())).bind("lhs"))))))))))
    .bind("build_func"),
  // Inliner v1.
  functionDecl(
    matchesName("BUILD_cc_.*"),
    isDefinition(),
    eachOf(
      forEachDescendant(binaryOperator(
        hasOperatorName("="),
        hasLHS(declRefExpr(to(parmVarDecl())).bind("lhs")),
        hasRHS(ignoringImpCasts(stringLiteral().bind("rhs"))))),
      forEachDescendant(binaryOperator(
        hasOperatorName("="),
        hasLHS(declRefExpr(to(parmVarDecl())).bind("lhs")),
        hasRHS(ignoringImpCasts(integerLiteral().bind("rhs"))))),
      forEachDescendant(binaryOperator(
        hasOperatorName("="),
        hasLHS(declRefExpr(to(parmVarDecl())).bind("lhs")),
        hasRHS(ignoringImpCasts(cxxBoolLiteral().bind("rhs"))))),
      forEachDescendant(cxxOperatorCallExpr(
        hasOverloadedOperatorName("="),
        hasDescendant(declRefExpr(to(parmVarDecl())).bind("lhs")),
        hasDescendant(initListExpr().bind("rhs"))))))
    .bind("build_func"),
  // Inliner v1 with no attribute assignments.
  functionDecl(
    matchesName("BUILD_cc_.*"),
    isDefinition(),
    unless(eachOf(
      forEachDescendant(binaryOperator(
        hasOperatorName("="),
        hasLHS(declRefExpr(to(parmVarDecl())).bind("lhs")),
        hasRHS(ignoringImpCasts(stringLiteral().bind("rhs"))))),
      forEachDescendant(binaryOperator(
        hasOperatorName("="),
        hasLHS(declRefExpr(to(parmVarDecl())).bind("lhs")),
        hasRHS(ignoringImpCasts(integerLiteral().bind("rhs"))))),
      forEachDescendant(binaryOperator(
        hasOperatorName("="),
        hasLHS(declRefExpr(to(parmVarDecl())).bind("lhs")),
        hasRHS(ignoringImpCasts(cxxBoolLiteral().bind("rhs"))))),
      forEachDescendant(cxxOperatorCallExpr(
        hasOverloadedOperatorName("="),
        hasDescendant(declRefExpr(to(parmVarDecl())).bind("lhs")),
        hasDescendant(initListExpr().bind("rhs")))))))
    .bind("build_func"));

class BuildPrinter : public MatchFinder::MatchCallback {
public:
  BuildPrinter() : buildRule_(NULL) {}

  void Print() const {
    for (const auto &rule : buildRules_) {
      if (rule.rule() == "" || rule.name() == "") {
        // Ignore incomplete rules.
        continue;
      }

      if (&rule != &buildRules_.front()) {
        llvm::outs() << "\n";
      }

      llvm::outs() << rule.rule() << "(\n";

      const Descriptor *descriptor = rule.GetDescriptor();
      const Reflection *reflection = rule.GetReflection();
      const int field_count = descriptor->field_count();
      for (int i = 0; i < field_count; ++i) {
        const FieldDescriptor *field = descriptor->field(i);
        if (!field->is_repeated() && !reflection->HasField(rule, field)) {
          continue;
        }
        switch (field->cpp_type()) {
          case FieldDescriptor::CPPTYPE_UINT32: {
            if (field->name() == "rulehash") {
              // Don't print out the rulehash field.
              continue;
            }
            auto value = reflection->GetUInt32(rule, field);
            if (value > 0) {
              llvm::outs() << "    " << field->name() << " = " << value << ",\n";
            }
            break;
          }
          case FieldDescriptor::CPPTYPE_INT32: {
            auto value = reflection->GetInt32(rule, field);
            if (value != 0) {
              llvm::outs() << "    " << field->name() << " = " << value << ",\n";
            }
            break;
          }
          case FieldDescriptor::CPPTYPE_UINT64: {
            auto value = reflection->GetUInt64(rule, field);
            if (value > 0) {
              llvm::outs() << "    " << field->name() << " = " << value << ",\n";
            }
            break;
          }
          case FieldDescriptor::CPPTYPE_INT64: {
            auto value = reflection->GetInt64(rule, field);
            if (value != 0) {
              llvm::outs() << "    " << field->name() << " = " << value << ",\n";
            }
            break;
          }
          case FieldDescriptor::CPPTYPE_BOOL: {
            int value = reflection->GetBool(rule, field) ? 1 : 0;
            if (value) {
              llvm::outs() << "    " << field->name() << " = " << value << ",\n";
            }
            break;
          }
          case FieldDescriptor::CPPTYPE_STRING: {
            if (field->name() == "rule") {
              // Don't print out the meta rule field.
              continue;
            }
            if (field->is_repeated()) {
              int field_size = reflection->FieldSize(rule, field);

              bool isDeps = (field->name() == "deps");

              int fieldCount = field_size + (isDeps ? 1 : 0);

              if (fieldCount > 0) {
                llvm::outs() << "    " << field->name() << " = [\n";
                if (isDeps) {
                  llvm::outs() << "        \"" << RULES2_DEP << "\",\n";
                }
                for (int k = 0; k < field_size; ++k) {
                  auto value = reflection->GetRepeatedString(rule, field, k);
                  if (!isDeps || (value != RULES1_DEP && value != RULES2_DEP)) {
                    llvm::outs() << "        \"" << value << "\",\n";
                  }
                }
                llvm::outs() << "    ],\n";
              }
            } else {
              auto value = reflection->GetString(rule, field);
              if (!value.empty()) {
                llvm::outs() << "    " << field->name() << " = \"" << value << "\",\n";
              }
            }
            break;
          }
          default: {
            llvm::errs() << "Unsupported field type " << field->cpp_type() << " in attr "
                         << field->name() << " of rule " << rule.rule();
            throw std::runtime_error("Aborting");
          }
        }
      }
      llvm::outs() << ")\n# cc_end(";
      llvm::write_hex(llvm::outs(), rule.rulehash(), llvm::HexPrintStyle::PrefixLower, 10);
      llvm::outs() << ")\n";
    }
  }

  static string StripSuffix(const string &input) {
    size_t dot = input.find_last_of(".");
    if (dot == string::npos) {
      return input;
    }
    return input.substr(0, dot);
  }

  static string getBasename(const string &input) {
    size_t slash = input.find_last_of("/");
    if (slash == string::npos) {
      return input;
    }
    return input.substr(slash + 1);
  }

  virtual void run(const MatchFinder::MatchResult &Result) {
    if (const FunctionDecl *decl = Result.Nodes.getNodeAs<FunctionDecl>("build_func")) {
      auto &sourceManager = Result.Context->getSourceManager();
      auto sourceLocation = decl->getBeginLoc();
      auto expansionLocation = sourceManager.getExpansionLoc(sourceLocation);
      auto spellingLocation = sourceManager.getSpellingLoc(sourceLocation);
      const string sourceFile =
        Result.Context->getSourceManager().getFilename(expansionLocation).str();
      const string rulesFile =
        Result.Context->getSourceManager().getFilename(spellingLocation).str();

      const string rulesName = getBasename(rulesFile);

      const string sourceName = getBasename(sourceFile);
      if (buildMethod_ != decl->getName() || sourceName_ != sourceName) {

        sourceName_ = sourceName;
        buildMethod_ = decl->getName().str();
        buildRules_.emplace_back(BuildRule());
        buildRule_ = &buildRules_.back();
        const string defaultName = StripSuffix(sourceName_);
        buildRule_->set_rulehash(ce_mm3::mm3_x86_32(defaultName.data(), defaultName.size(), 0));
        buildRule_->set_name(defaultName);

        if (rulesName == "rules.h") {
          if (!buildMethod_.compare(0, 16, "BUILD_cc_library")) {
            buildRule_->set_rule("cc_library");
          } else if (!buildMethod_.compare(0, 15, "BUILD_cc_binary")) {
            buildRule_->set_rule("cc_binary");
          } else if (!buildMethod_.compare(0, 13, "BUILD_cc_test")) {
            buildRule_->set_rule("cc_test");
          } else {
            llvm::errs() << "Unknown BUILD rule " << buildMethod_ << "\n";
            throw std::runtime_error("Aborting");
          }
        } else {
          // Inliner rule should be a macro expansion.
          assert(sourceLocation.isMacroID());
          auto macroLocation = sourceLocation;

          while (sourceManager.isMacroArgExpansion(macroLocation)) {
            macroLocation = sourceManager.getImmediateExpansionRange(macroLocation).getBegin();
          }
          macroLocation = sourceManager.getSpellingLoc(
            sourceManager.getImmediateExpansionRange(macroLocation).getBegin());

          std::pair<FileID, unsigned> ExpansionInfo = sourceManager.getDecomposedLoc(macroLocation);
          unsigned macroTokenLength =
            Lexer::MeasureTokenLength(macroLocation, sourceManager, LangOptions());
          StringRef ExpansionBuffer = sourceManager.getBufferData(ExpansionInfo.first);
          buildRule_->set_rule(
            ExpansionBuffer.substr(ExpansionInfo.second, macroTokenLength).str());
        }

        buildRule_->add_srcs(sourceName_);
      }
    }

    if (!buildRule_) {
      return;
    }

    if (const DeclRefExpr *decl = Result.Nodes.getNodeAs<DeclRefExpr>("lhs")) {
      const string &key = decl->getNameInfo().getName().getAsString();
      const FieldDescriptor *field = buildRule_->GetDescriptor()->FindFieldByName(key);
      const Reflection *reflection = buildRule_->GetReflection();

      if (!field) {
        llvm::errs() << "Unknown field " << key << " in rule " << buildRule_->rule();
        throw std::runtime_error("Aborting");
      }

      const auto updateIntHash = [](uint32_t hash, int32_t value) -> uint32_t {
        return hash ^ (value + 0x9e3779b9 + (hash << 6) + (value >> 2));
      };

      if (const clang::StringLiteral *rhs = Result.Nodes.getNodeAs<clang::StringLiteral>("rhs")) {
        const string &value = rhs->getString().str();
        reflection->SetString(buildRule_, field, value);
        buildRule_->set_rulehash(
          ce_mm3::mm3_x86_32(value.data(), value.size(), buildRule_->rulehash()));
      } else if (
        const clang::IntegerLiteral *rhs = Result.Nodes.getNodeAs<clang::IntegerLiteral>("rhs")) {

        const auto &value = rhs->getValue();
        switch (field->type()) {
          case FieldDescriptor::TYPE_SFIXED32:
          case FieldDescriptor::TYPE_INT32:
          case FieldDescriptor::TYPE_SINT32: {
            const int64_t intValue = value.getSExtValue();
            reflection->SetInt64(buildRule_, field, intValue);
            buildRule_->set_rulehash(
              updateIntHash(buildRule_->rulehash(), static_cast<int32_t>(intValue)));
            break;
          }
          case FieldDescriptor::TYPE_SFIXED64:
          case FieldDescriptor::TYPE_INT64:
          case FieldDescriptor::TYPE_SINT64: {
            const int32_t intValue = value.getSExtValue();
            reflection->SetInt32(buildRule_, field, intValue);
            buildRule_->set_rulehash(
              updateIntHash(buildRule_->rulehash(), static_cast<int32_t>(intValue)));
            break;
          }
          case FieldDescriptor::TYPE_FIXED64:
          case FieldDescriptor::TYPE_UINT64: {
            const uint64_t intValue = value.getZExtValue();
            reflection->SetUInt64(buildRule_, field, intValue);
            buildRule_->set_rulehash(
              updateIntHash(buildRule_->rulehash(), static_cast<int32_t>(intValue)));
            break;
          }
          case FieldDescriptor::TYPE_FIXED32:
          case FieldDescriptor::TYPE_UINT32: {
            const uint32_t intValue = value.getZExtValue();
            reflection->SetUInt32(buildRule_, field, intValue);
            buildRule_->set_rulehash(
              updateIntHash(buildRule_->rulehash(), static_cast<int32_t>(intValue)));
            break;
          }
          case FieldDescriptor::TYPE_BOOL: {
            const bool boolValue = value.getBoolValue();
            reflection->SetBool(buildRule_, field, boolValue);
            buildRule_->set_rulehash(updateIntHash(buildRule_->rulehash(), boolValue ? 1 : 0));
            break;
          }
          default:
            // Ignore non integer enumeration values.
            break;
        }
      } else if (
        const clang::CXXBoolLiteralExpr *rhs =
          Result.Nodes.getNodeAs<clang::CXXBoolLiteralExpr>("rhs")) {
        if (field->type() == FieldDescriptor::TYPE_BOOL) {
          // Allow bools for bool fields.
          const bool boolValue = rhs->getValue();
          reflection->SetBool(buildRule_, field, boolValue);
          buildRule_->set_rulehash(updateIntHash(buildRule_->rulehash(), boolValue ? 1 : 0));
        }
      } else if (const InitListExpr *expr = Result.Nodes.getNodeAs<InitListExpr>("rhs")) {
        for (auto iter = expr->begin(); iter != expr->end(); iter++) {
          // Use llvm RTTI to traverse the first children, until encountering the first
          // StringLiteral.
          Expr *expr = dyn_cast<Expr>(*iter);
          while (expr) {
            if (clang::StringLiteral *str = dyn_cast<clang::StringLiteral>(expr)) {
              const string &value = str->getString().str();
              if (
                (key != "srcs" || value != sourceName_) && value != RULES1_DEP
                && value != RULES2_DEP) {
                // Skip over the implicit self-source or inliner rules but add any
                // additional source files.
                reflection->AddString(buildRule_, field, value);
                buildRule_->set_rulehash(
                  ce_mm3::mm3_x86_32(value.data(), value.size(), buildRule_->rulehash()));
              }
            }
            if (expr->child_begin() != expr->child_end()) {
              // Traverse to the first child.
              expr = dyn_cast<Expr>(*expr->child_begin());
            } else {
              expr = nullptr;
            }
          }
        }
      }
    }
  }

private:
  BuildRule *buildRule_;
  vector<BuildRule> buildRules_;
  string sourceName_;
  string buildMethod_;
};

// Apply a custom category to all command-line options so that they are the
// only ones displayed.
static llvm::cl::OptionCategory g_tool_category("parse-build-rules options");
static cl::opt<bool> showDiagnostics(
  "v", cl::desc("Verbose output, with compile warnings and errors."), cl::cat(g_tool_category));

static cl::opt<bool> stdinMode(
  "stdin",
  cl::desc("Override source file's content and read stdin"),
  cl::init(false),
  cl::cat(g_tool_category));

// A help message for this specific tool can be added afterwards.
static cl::extrahelp MoreHelp(
  "\nParses Bazel build rules embedded inline into C++ files. Must be run from "
  "the WORKSPACE directory.\n");

int main(int argc, const char **argv) {
  auto optionsParser = tooling::CommonOptionsParser::create(argc, argv, g_tool_category);

  if (!optionsParser) {
    llvm::errs() << optionsParser.takeError();
    return 1;
  }

  ClangTool tool(optionsParser->getCompilations(), optionsParser->getSourcePathList());

  llvm::StringRef sourceFilePath = optionsParser->getSourcePathList().front();
  std::unique_ptr<llvm::MemoryBuffer> code;
  if (stdinMode) {
    assert(
      optionsParser->getSourcePathList().size() == 1
      && "Expect exactly one file name when reading from stdin stdinMode.");
    llvm::ErrorOr<std::unique_ptr<llvm::MemoryBuffer>> codeOrErr = MemoryBuffer::getSTDIN();
    if (std::error_code EC = codeOrErr.getError()) {
      errs() << EC.message() << "\n";
      return 1;
    }
    code = std::move(codeOrErr.get());
    if (code->getBufferSize() == 0)
      return 0;  // Skip empty files.

    tool.mapVirtualFile(sourceFilePath, code->getBuffer());
  }

  if (!showDiagnostics) {
    tool.setDiagnosticConsumer(new IgnoringDiagConsumer());
  }

  BuildPrinter printer;
  MatchFinder finder;
  finder.addMatcher(matcher, &printer);

  const int status = tool.run(newFrontendActionFactory(&finder).get());

  if (status == 0) {
    printer.Print();
  }

  return status;
}
