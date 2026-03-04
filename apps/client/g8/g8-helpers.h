#pragma once

#include <capnp/pretty-print.h>

#include <CLI/CLI.hpp>
#include <map>

#include "c8/c8-log.h"
#include "c8/git/g8-api.capnp.h"
#include "c8/string.h"
#include "c8/vector.h"

// Namespace
namespace {
constexpr const char DIM[] = "\033[2m";
constexpr const char RED[] = "\033[31m";
constexpr const char GREEN[] = "\033[32m";
constexpr const char CYAN[] = "\033[36m";
constexpr const char MAGENTA[] = "\033[35m";
constexpr const char YELLOW[] = "\033[33m";
constexpr const char CLEAR[] = "\033[0m";
constexpr const char BOLD[] = "\033[1m";

template <typename Func, class Context, class G8RequestType>
auto verboseCheck(const Context &ctx, const G8RequestType &req, Func func) {
  if (ctx.verbose) {
    c8::C8Log("Request:\n%s", capnp::prettyPrint(req.reader()).flatten().cStr());
  }
  auto res = func(req.reader());
  if (ctx.verbose) {
    c8::C8Log("Response:\n%s", capnp::prettyPrint(res.reader()).flatten().cStr());
  }
  if (0 != res.reader().getStatus().getCode()) {
    c8::C8Log("ERROR: %s", res.reader().getStatus().getMessage().cStr());
    throw CLI::RuntimeError(res.reader().getStatus().getCode());
  }
  return res;
}

// NOTE(pawel) It is assumed that ctx already exists in the local scope.
#define CHECK_DECLARE_G8(res, ...) auto res = verboseCheck(ctx, __VA_ARGS__)
#define CHECK_G8(res, ...) res = verboseCheck(ctx, __VA_ARGS__)
}  // namespace

// Defined in main() and passed to all sub commands;
struct MainContext {
  bool verbose{false};
  c8::String repoPath;
};

// Options for the g8 ls command.
struct LsOptions {
  bool longFormat = false;
  bool all = false;
  bool humanReadable = false;
  bool onePerLine = false;
  bool reverse = false;
  bool sortByTime = false;
  bool sortBySize = false;
};

c8::String repoPath();

c8::String repoPathOrEmpty();

c8::String dataString(const c8::Vector<uint8_t> &data);

double epochMicros();

void read8thWallCreds(const c8::String &host, c8::String *jwt, c8::String *err);

c8::String resolveEditor();

c8::String resolveMergeTool();

c8::String resolveDiffTool();

bool is8wHost(const c8::String &host);

const std::pair<c8::String, c8::String> getCredentialsFromSecureStorageForHost(
  const c8::String &host);

void setCredsForHost(c8::G8Authentication::Builder creds, const c8::String &host);

c8::String urlHost(const c8::String &url);

c8::String repoHost(const c8::String &repo);

void setCredsForCwdRepo(c8::G8Authentication::Builder creds);

std::optional<c8::G8Changeset::Reader> extractChangeset(
  c8::G8ChangesetResponse::Reader reader, const c8::String &changesetId);

bool isChangesetDirty(c8::G8Changeset::Reader changeset);

c8::Vector<c8::String> extractChangesetPaths(c8::G8Changeset::Reader changeset);

bool parseUserConfig(std::map<std::string, std::vector<std::string>> &groups);
