// Copyright (c) 2022 8th Wall, LLC.
// Author: Pawel Czarnecki (pawelczarnecki@nianticlabs.com)

#include "bzl/inliner/rules2.h"

cc_library {
  hdrs = {
    "g8-impl.h",
  };
  deps = {
    ":g8-helpers",
    ":g8-plumbing",
    "//c8:c8-log",
    "//c8:exceptions",
    "//c8/git:g8-api.capnp-cc",
    "//c8/git:g8-git",
    "//c8/stats:scope-timer",
    "//c8/string:format",
    "//c8/string:strcat",
    "//c8/string:trim",
    "//c8:process",
    "//c8:map",
    "//c8:vector",
  };
}
cc_end(0x58dcd2ac);

#include <grp.h>
#include <pwd.h>
#include <sys/ioctl.h>  // for ioctl and TIOCGWINSZ
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>  // for STDOUT_FILENO

#include <ctime>
#include <string_view>

#include "apps/g8/g8-helpers.h"
#include "apps/g8/g8-impl.h"
#include "apps/g8/g8-plumbing.h"
#include "c8/c8-log.h"
#include "c8/exceptions.h"
#include "c8/git/g8-api.capnp.h"
#include "c8/git/g8-git.h"
#include "c8/map.h"
#include "c8/process.h"
#include "c8/stats/scope-timer.h"
#include "c8/string.h"
#include "c8/string/format.h"
#include "c8/string/strcat.h"
#include "c8/string/trim.h"
#include "c8/vector.h"

namespace fs = std::filesystem;
namespace ch = std::chrono;
using namespace c8;

namespace {
const char *getFileUserName(uid_t uid) {
  struct passwd *pw = getpwuid(uid);
  if (pw) {
    return pw->pw_name;
  } else {
    return "unknown";
  }
}

const char *getFileGroupName(gid_t gid) {
  struct group *gr = getgrgid(gid);
  if (gr) {
    return gr->gr_name;
  } else {
    return "unknown";
  }
}

String epochToDateString(int64_t epoch) {
  auto now = ch::system_clock::now();
  auto nowDays = ch::floor<ch::days>(now);
  ch::year_month_day today = nowDays;
  ch::year_month_day sixMonthsLater = today + ch::months(6);
  ch::year_month_day sixMonthsAgo = today - ch::months(6);

  auto timeOfDay = now - nowDays;

  ch::system_clock::time_point epochTime = ch::system_clock::from_time_t(epoch);

  ch::system_clock::time_point sixMonthsLaterTime =
    std::chrono::sys_days{sixMonthsLater} + timeOfDay;
  ch::system_clock::time_point sixMonthsAgoTime = std::chrono::sys_days{sixMonthsAgo} + timeOfDay;

  std::time_t epochTimeT = epoch;
  std::tm *timeInfo = std::localtime(&epochTimeT);

  char buffer[20];
  if (epochTime >= sixMonthsLaterTime || epochTime <= sixMonthsAgoTime) {
    std::strftime(buffer, sizeof(buffer), "%b %e  %Y", timeInfo);
  } else {
    std::strftime(buffer, sizeof(buffer), "%b %e %H:%M", timeInfo);
  }

  return String(buffer);
}

}  // namespace

void keepHouseImpl(MainContext &ctx) {
  MutableRootMessage<G8RepositoryRequest> req;
  req.builder().setRepo(ctx.repoPath);
  req.builder().setUser(getenv("USER"));
  req.builder().setAction(G8RepositoryRequest::Action::KEEP_HOUSE);
  CHECK_DECLARE_G8(res, req, c8::g8Repository);
}

void inspectImpl(MainContext &ctx, const String &inspectPoint, const String &inspectRegex) {
  MutableRootMessage<G8InspectRequest> req;
  req.builder().setRepo(ctx.repoPath);
  req.builder().setInspectPoint(inspectPoint);
  req.builder().setInspectRegex(inspectRegex);
  CHECK_DECLARE_G8(res, req, c8::g8Inspect);
  for (auto info : res.reader().getInfo()) {
    C8Log("%s %s", info.getBlobId().cStr(), info.getPath().cStr());
  }
}

void cloneImpl(
  MainContext &ctx,
  const String &name,
  const String &host,
  const String &dir,
  const String &cloneCheckoutId) {
  MutableRootMessage<G8RepositoryRequest> req;
  req.builder().setRepo(dir);
  req.builder().setUrl(name);
  req.builder().setUser(getenv("USER"));
  setCredsForHost(req.builder().getAuth(), urlHost(host));

  if (!cloneCheckoutId.empty()) {
    req.builder().setCloneCheckoutId(cloneCheckoutId);
  }

  CHECK_DECLARE_G8(res, req, c8::g8Repository);

  C8Log("Cloned repo %s", res.reader().getRepo().cStr());
}

void newClientImpl(MainContext &ctx, const Vector<String> &clientNames) {
  MutableRootMessage<G8ClientRequest> req;
  req.builder().setRepo(repoPath());
  req.builder().setAction(G8ClientRequest::Action::CREATE);
  req.builder().initClient(clientNames.size());
  setCredsForCwdRepo(req.builder().getAuth());

  for (int i = 0; i < clientNames.size(); ++i) {
    req.builder().getClient().set(i, clientNames[i].c_str());
  }
  req.builder().setUser(getenv("USER"));

  CHECK_DECLARE_G8(res, req, c8::g8Client);

  if (res.reader().getStatus().getCode() == 0) {
    // Now switch to the last client in the create request.
    req.builder().setAction(G8ClientRequest::Action::SWITCH);
    String lastClient = req.builder().getClient()[req.builder().getClient().size() - 1].cStr();
    req.builder().initClient(1).set(0, lastClient.c_str());
    CHECK_G8(res, req, c8::g8Client);

    for (auto client : res.reader().getClient()) {
      if (client.getActive()) {
        C8Log("Switched to new client \033[32m%s\033[0m", client.getName().cStr());
      }
    }
  }
}

void presubmitImpl(MainContext &ctx, String changesetId) {
  auto res = listChangesets(ctx, false);
  auto changesets = res.reader().getChangeset();

  if (changesetId.empty() && changesets.size() == 2) {
    for (auto changeset : changesets) {
      if (changeset.getId() != "working") {
        changesetId = changeset.getId().cStr();
      }
    }
  } else if (changesetId.empty() && changesets.size() >= 2) {
    C8Log("Multiple changesets found, please specify a changeset id");
    return;
  } else if (changesetId.empty()) {
    C8Log("No changesets found to execute presubmit");
    return;
  }

  auto changeset = extractChangeset(res.reader(), changesetId);
  if (!changeset) {
    C8Log("Could not find changeset %s", changesetId.c_str());
    exit(1);
  }

  if (isChangesetDirty(*changeset)) {
    C8Log("Changeset is dirty, update changeset");
    return;
  }

  auto presubmitFilePath = fs::path(repoPath()) / "presubmit";

  if (!fs::exists(presubmitFilePath)) {
    C8Log("Could not find \033[32mpresubmit\033[0m in repo root");
    exit(1);
  }

  auto repoName = res.reader().getRemote().getRepoName().cStr();
  auto worktreePath = fs::path("/tmp/g8presubmit") / repoName;
  auto commitId = changeset->getCommitId().cStr();

  if (fs::exists(worktreePath)) {
    process::ExecuteOptions executeOptions;
    executeOptions.file = "git";
    executeOptions.workDirectory = worktreePath;

    auto checkoutRes =
      process::execute(executeOptions, {"checkout", commitId}).process.getExitCode();
    if (checkoutRes != 0) {
      C8Log(
        "Unable to checkout changeset changes into temp location, git returned %d", checkoutRes);
      exit(checkoutRes);
    }
  } else {
    process::executeNoRedirect("git", "worktree", "prune").getExitCode();  // waits until completion
    auto addRes =
      process::executeNoRedirect("git", "worktree", "add", worktreePath.c_str(), commitId)
        .getExitCode();
    if (addRes != 0) {
      C8Log(
        "Unable to create worktree with changeset changes at temp location, git returned %d",
        addRes);
      exit(addRes);
    }
  }

  auto changesetPaths = extractChangesetPaths(*changeset);

  process::ExecuteOptions executeOptions;
  executeOptions.file = presubmitFilePath;
  executeOptions.workDirectory = worktreePath;

  auto presubmitRes = process::execute(executeOptions, changesetPaths).process.getExitCode();
  if (presubmitRes != 0) {
    C8Log(
      "Unable to run presubmit on changeset changes at temp location, git returned %d",
      presubmitRes);
    exit(presubmitRes);
  }
}

void syncMasterImpl(MainContext &ctx) {
  MutableRootMessage<G8RepositoryRequest> req;
  req.builder().setRepo(repoPath());
  req.builder().setUser(getenv("USER"));
  setCredsForCwdRepo(req.builder().getAuth());
  req.builder().setAction(G8RepositoryRequest::Action::SYNC_MASTER);
  CHECK_DECLARE_G8(res, req, c8::g8Repository);
}

int getTerminalWidth() {
  struct winsize w;
  ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
  return w.ws_col;
}

bool isStdoutTerminal() { return isatty(STDOUT_FILENO); }

void lsPrint(const LsOptions &opts, Vector<G8FileInfo::Reader> &files) {
  constexpr int COL_SPACING = 7;

  const int isTerminal = isStdoutTerminal();

  const int termWidth = isTerminal ? getTerminalWidth() : 0;
  fs::path currentPath(fs::current_path());
  fs::path basePath(repoPath());

  Vector<String> paths(files.size());
  std::transform(
    files.begin(),
    files.end(),
    paths.begin(),
    [&basePath, &currentPath, &opts](const G8FileInfo::Reader &file) {
      fs::path path = file.getPath().cStr();
      std::string_view label = file.getLabel().cStr();
      fs::path relativeLabelPath;
      if (label.empty()) {
        // Give the relative path if there is no label.
        relativeLabelPath = (basePath / file.getPath().cStr()).lexically_relative(currentPath);
      } else {
        // Give the path relative to the label.
        relativeLabelPath = (basePath / file.getPath().cStr()).lexically_relative(basePath / label);
      }

      if (opts.longFormat && S_ISLNK(file.getMode())) {
        // For symlinks, include the link tafget in the output.
        String linkTarget = file.getSymlinkTarget().cStr();
        if (linkTarget.empty()) {
          linkTarget = "unknown";
        }
        return relativeLabelPath.native() + " -> " + linkTarget;
      } else {
        return relativeLabelPath.native();
      }
    });

  const bool singleColumn = opts.longFormat || opts.onePerLine || !isTerminal;

  auto longest = std::max_element(paths.begin(), paths.end(), [](const String &a, const String &b) {
    return a.length() < b.length();
  });

  size_t linkCountMaxChars = 0;
  size_t userMaxChars = 0;
  size_t groupMaxChars = 0;
  size_t byteSizeMaxChars = 0;

  for (const auto &file : files) {
    linkCountMaxChars = std::max(linkCountMaxChars, std::to_string(file.getLinkCount()).length());
    userMaxChars = std::max(userMaxChars, std::strlen(getFileUserName(file.getUid())));
    groupMaxChars = std::max(groupMaxChars, std::strlen(getFileGroupName(file.getGid())));
    byteSizeMaxChars = std::max(byteSizeMaxChars, std::to_string(file.getByteSize()).length());
  }

  const int numCols = singleColumn ? 1 : termWidth / (longest->length() + COL_SPACING);
  const int numRows = singleColumn ? paths.size() : (paths.size() + numCols - 1) / numCols;

  for (int row = 0; row < numRows; ++row) {
    String rowOutput;
    for (int col = 0; col < numCols; ++col) {
      const int index = row + col * numRows;
      if (index < paths.size()) {
        G8FileInfo::Reader file = files[index];
        const auto &path = paths[index];

        const String displayPath =
          (file.getSparseNoCheckout() && isTerminal) ? "\033[90m" + path + "\033[0m" : path;

        char fileType;
        switch (file.getMode() & S_IFMT) {
          case S_IFDIR:
            fileType = 'd';
            break;
          case S_IFREG:
            fileType = '-';
            break;
          case S_IFLNK:
            fileType = 'l';
            break;
          case S_IFBLK:
            fileType = 'b';
            break;
          case S_IFCHR:
            fileType = 'c';
            break;
          case S_IFIFO:
            fileType = 'p';
            break;
          case S_IFSOCK:
            fileType = 's';
            break;
          case S_IFWHT:
            fileType = 'w';
            break;
          default:
            fileType = '?';
        }

        if (opts.longFormat) {
          rowOutput += format(
            "%c%c%c%c%c%c%c%c%c%c  %*d %-*s  %-*s  %*d %s %s",
            fileType,
            (file.getMode() & S_IRUSR) ? 'r' : '-',
            (file.getMode() & S_IWUSR) ? 'w' : '-',
            (file.getMode() & S_IXUSR) ? 'x' : '-',
            (file.getMode() & S_IRGRP) ? 'r' : '-',
            (file.getMode() & S_IWGRP) ? 'w' : '-',
            (file.getMode() & S_IXGRP) ? 'x' : '-',
            (file.getMode() & S_IROTH) ? 'r' : '-',
            (file.getMode() & S_IWOTH) ? 'w' : '-',
            (file.getMode() & S_IXOTH) ? 'x' : '-',
            linkCountMaxChars,
            file.getLinkCount(),
            userMaxChars,
            getFileUserName(file.getUid()),
            groupMaxChars,
            getFileGroupName(file.getGid()),
            byteSizeMaxChars,
            file.getByteSize(),
            epochToDateString(file.getMtime()).c_str(),
            displayPath.c_str());
        } else {
          if (col == numCols - 1) {
            rowOutput += displayPath;
          } else {
            // Add spaces to the end of the path to align the next column.
            rowOutput += displayPath + String(longest->length() + COL_SPACING - path.length(), ' ');
          }
        }
      }
    }
    // Print the entire row at once.
    C8Log("%s", rowOutput.c_str());  // Print the entire row at once.
  }
};

void lsImpl(MainContext &ctx, const LsOptions &opts, const c8::Vector<c8::String> &paths) {
  MutableRootMessage<G8FileRequest> req;
  req.builder().setRepo(repoPath());
  req.builder().setAction(G8FileRequest::Action::LIST);
  req.builder().setIncludeDotFiles(opts.all);

  fs::path basePath(repoPath());
  fs::path currentPath(fs::current_path());

  if (!paths.empty()) {
    req.builder().initPaths(paths.size());
    for (int i = 0; i < paths.size(); ++i) {
      fs::path fullPath = currentPath / paths[i];

      // If fullPath is not within repoPath(), then return an error.
      fs::path relativePath = fs::relative(fullPath, basePath);

      // If the relative path is not within the repo, then return an error.
      if (relativePath.native().starts_with("..")) {
        throw InvalidArgument(
          format("Path %s is not within the current repository", paths[i].c_str()));
      }

      // Get a relative path without canonicalization.
      fs::path lexicallyRelativePath = fullPath.lexically_relative(basePath);

      req.builder().getPaths().set(i, lexicallyRelativePath.c_str());
    }
  } else {
    // Add the current directory if no operands are given.
    fs::path relativePath = fs::relative(currentPath, basePath);
    req.builder().initPaths(1).set(0, relativePath.c_str());
  }

  if (ctx.verbose) {
    c8::C8Log("Request:\n%s", capnp::prettyPrint(req.reader()).flatten().cStr());
  }
  auto res = g8FileRequest(req.reader());
  if (ctx.verbose) {
    c8::C8Log("Response:\n%s", capnp::prettyPrint(res.reader()).flatten().cStr());
  }

  TreeMap<String, Vector<G8FileInfo::Reader>> filesByLabel;

  for (auto file : res.reader().getFiles().getInfo()) {
    Vector<G8FileInfo::Reader> &filesForLabel = filesByLabel[file.getLabel().cStr()];
    filesForLabel.push_back(file);
  }

  // Convert the TreeMap to a Vector of pairs for sorting.
  Vector<std::pair<String, Vector<G8FileInfo::Reader>>> filesByLabelVector;
  std::copy(filesByLabel.begin(), filesByLabel.end(), std::back_inserter(filesByLabelVector));

  // Sort the filesByLabelVector alphabetically by label.
  std::sort(filesByLabelVector.begin(), filesByLabelVector.end(), [](const auto &a, const auto &b) {
    return a.first < b.first;
  });

  bool firstGroup = true;

  for (auto &[label, files] : filesByLabelVector) {
    if (firstGroup) {
      firstGroup = false;
    } else {
      C8Log("");  // Print a newline between groups.
    }
    fs::path relativeLabelPath;
    if (!label.empty() && filesByLabelVector.size() > 1) {
      C8Log("%s:", label.c_str());
    }

    // Sort the files.
    if (opts.sortByTime) {
      std::sort(files.begin(), files.end(), [](const auto &a, const auto &b) {
        return b.getMtime() < a.getMtime();
      });
    } else if (opts.sortBySize) {
      std::sort(files.begin(), files.end(), [](const auto &a, const auto &b) {
        return b.getByteSize() < a.getByteSize();
      });
    } else {
      std::sort(files.begin(), files.end(), [](const auto &a, const auto &b) {
        return a.getPath() < b.getPath();
      });
    }

    if (opts.reverse) {
      std::reverse(files.begin(), files.end());
    }

    // Print the files.
    lsPrint(opts, files);

    for (const auto &file : files) {
      if (label.empty()) {
        // Give the relative path if there is no label.
        relativeLabelPath = fs::relative(basePath / file.getPath().cStr(), currentPath);
      } else {
        // Give the path relative to the label.
        relativeLabelPath =
          fs::relative(basePath / file.getPath().cStr(), basePath / file.getLabel().cStr());
      }
    }
  }
  if (0 != res.reader().getStatus().getCode()) {
    throw RuntimeError(strTrim(res.reader().getStatus().getMessage().cStr()));
  }
}
