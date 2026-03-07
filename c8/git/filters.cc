// Copyright (c) 2020 8th Wall, Inc.

#include "bzl/inliner/rules2.h"

cc_library {
  hdrs = {
    "filters.h",
  };
  visibility = {
    "//visibility:private",
  };
  deps = {
    "//c8:c8-log",
    "//c8/string:join",
    "//c8/string:strcat",
    "//c8:process",
    "//c8/string:format",
    "@libgit2//:libgit2",
  };
  copts = {"-Iexternal/libgit2/src"};
}
cc_end(0x6f9d5a83);

#include <git2.h>
#include <git2/sparse.h>
#include <git2/sys/filter.h>

#include <chrono>
#include <thread>

#include "c8/c8-log.h"
#include "c8/git/filters.h"
#include "c8/process.h"
#include "c8/string/format.h"
#include "c8/string/join.h"
#include "c8/string/strcat.h"

using namespace std::chrono_literals;

#define GIT_STATUS_RETURN(result) \
  {                               \
    if ((result) != 0) {          \
      return result;              \
    }                             \
  }

namespace {
// Returns a pointer that is within the bounds of str and is only valid so long as str is valid.
const char *maybeStripPrefix(const char *str, const char *prefix) {
  const char *res = str;
  const size_t prefixSize = prefix ? strlen(prefix) : 0;
  if (prefixSize > 0 && prefixSize <= strlen(str) && 0 == strncmp(prefix, str, prefixSize)) {
    res = str + prefixSize;
  }
  return res;
}
}  // namespace

namespace c8 {
namespace g8 {

#ifdef JAVASCRIPT
// TODO(pawel) consider how filters would work in the browser
int register_lfs_filter() { return 0; }
#else

namespace {
// Does not shell out smudge to git-lfs per file. Returns the pointer file and on filter shutdown
// runs git-lfs checkout.
constexpr bool WANT_EXTERNAL_SMUDGE = true;

// When a smudge operation occurs, set this flag so filter shutdown knows to run git-lfs checkout.
bool smudgeHappened = false;
String repoDirectory;

// Magic static for singleton list of files needing smudge is initialized the
// first time control passes through their declaration.
Vector<String> &filesNeedingSmudge() {
  static Vector<String> fns;
  return fns;
}

}  // namespace

class GitLfsFilter {
private:
  git_writestream parent;
  git_writestream *next;
  process::Pipe processInput;
  process::Pipe processOutput;

  git_filter_mode_t filterMode;

  GitLfsFilter(const git_filter_source *src, git_writestream *next) {
    parent.write = [](git_writestream *self, const char *buffer, size_t len) {
      return reinterpret_cast<GitLfsFilter *>(self)->write_(buffer, len);
    };
    parent.close = [](git_writestream *self) {
      return reinterpret_cast<GitLfsFilter *>(self)->close_();
    };
    parent.free = [](git_writestream *self) { delete self; };
    this->next = next;
    filterMode = git_filter_source_mode(src);

    const char *filename = maybeStripPrefix(
      git_filter_source_path(src), git_repository_workdir(git_filter_source_repo(src)));

    const char *op = filterMode == GIT_FILTER_SMUDGE ? "smudge" : "clean";

    // If deferring smudge, don't launch the git-lfs process here so write back the pointer file.
    if (WANT_EXTERNAL_SMUDGE && filterMode == GIT_FILTER_SMUDGE) {
      smudgeHappened = true;
      filesNeedingSmudge().push_back(filename);
      repoDirectory = git_repository_workdir(git_filter_source_repo(src));
      return;
    }

    process::Execute2Options executeOptions;
    executeOptions.file = "git-lfs";
    executeOptions.workDirectory = git_repository_workdir(git_filter_source_repo(src));

    std::tie(processInput, processOutput) = process::execute2(executeOptions, op, filename);
  }

  int write_(const char *buffer, size_t len) {
    if (WANT_EXTERNAL_SMUDGE && filterMode == GIT_FILTER_SMUDGE) {
      // Passes back the pointer file without smudging.
      next->write(next, buffer, len);
      return 0;
    }

    const auto bufferEnd = buffer + len;
    const char *bufferHead = buffer;

    while (true) {
      bufferHead = processInput.writeBytes(bufferHead, bufferEnd - bufferHead);
      if (bufferHead < bufferEnd) {
        std::this_thread::sleep_for(2ms);
        continue;
      }
      break;
    }
    return 0;
  }

  int close_() {
    if (WANT_EXTERNAL_SMUDGE && filterMode == GIT_FILTER_SMUDGE) {
      // Already sent back the pointer file in write_().
      next->close(next);
      return 0;
    }

    processInput.close();
    auto resultBytes = getRawOutput(processOutput);
    // NOTE(pawel) This can potentially be a large buffer; libgit2 should be able to handle it.
    next->write(next, reinterpret_cast<const char *>(resultBytes.data()), resultBytes.size());
    next->close(next);
    return 0;
  }

public:
  static int create(
    git_writestream **out,
    git_filter *self,
    void **payload,
    const git_filter_source *src,
    git_writestream *next) {
    *out = reinterpret_cast<git_writestream *>(new GitLfsFilter(src, next));
    return 0;
  };

  static int check(
    git_filter *self,
    void **payload, /* points to NULL ptr on entry, may be set */
    const git_filter_source *src,
    const char **attr_values) {
    const char *filename = maybeStripPrefix(
      git_filter_source_path(src), git_repository_workdir(git_filter_source_repo(src)));
    int checkout = -1;
    GIT_STATUS_RETURN(git_sparse_check_path(&checkout, git_filter_source_repo(src), filename));

    if (checkout == 1) {
      // The file is checked out in the sparse checkout, so ok to filter.
      return 0;
    } else {
      // The file is not checked out in the sparse checkout, so skip filtering.
      return GIT_PASSTHROUGH;
    }
  }
};

// Standard layout types can be consumed by C programs.
// A pointer to a standard-layout class may be converted (with reinterpret_cast) to a pointer to its
// first non-static data member and vice versa.
static_assert(std::is_standard_layout<GitLfsFilter>());

// Assuming libgit2 is already loaded.
// NOTE(pawel) Because process does not support javascript so we don't attempt to launch any.
int register_lfs_filter() {
  static git_filter lfs_filter{};
  GIT_STATUS_RETURN(git_filter_init(&lfs_filter, GIT_FILTER_VERSION));

  lfs_filter.attributes = "filter=lfs diff=lfs merge=lfs";
  lfs_filter.check = GitLfsFilter::check;
  lfs_filter.stream = GitLfsFilter::create;

  lfs_filter.initialize = [](git_filter *self) -> int {
    smudgeHappened = false;
    return 0;
  };
  lfs_filter.shutdown = [](git_filter *self) -> void {
    if (WANT_EXTERNAL_SMUDGE && smudgeHappened) {
      process::ExecuteOptions opts;
      opts.file = "git-lfs";
      opts.workDirectory = repoDirectory;

      const auto &fns = filesNeedingSmudge();

      // Fetch in batches 2048 LFS files at a time to avoid hitting the command line length limit.
      constexpr int BATCH_SIZE = 2048;
      for (size_t i = 0; i < fns.size(); i += BATCH_SIZE) {
        const auto start = fns.begin() + i;
        const auto end = std::min(start + BATCH_SIZE, fns.end());
        Vector<String> fetchArgs = {"fetch", strCat("--include=", strJoin(start, end, ","))};
        process::execute(opts, fetchArgs);
      }

      // Checkout in batches 2048 LFS files at a time to avoid hitting the command line length
      // limit.
      for (size_t i = 0; i < fns.size(); i += BATCH_SIZE) {
        const auto start = fns.begin() + i;
        const auto end = std::min(start + BATCH_SIZE, fns.end());
        Vector<String> checkoutArgs = {"checkout"};
        checkoutArgs.insert(checkoutArgs.end(), start, end);
        process::execute(opts, checkoutArgs);
      }

      smudgeHappened = false;
      filesNeedingSmudge().clear();
    }
  };

  GIT_STATUS_RETURN(git_filter_register("lfs", &lfs_filter, 800));
  return 0;
}

#endif  // JAVASCRIPT
}  // namespace g8
}  // namespace c8
