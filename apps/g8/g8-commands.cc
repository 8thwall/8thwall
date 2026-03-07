// Copyright (c) 2022 8th Wall, LLC.
// Author: Pawel Czarnecki (pawelczarnecki@nianticlabs.com)

#include "bzl/inliner/rules2.h"

cc_library {
  hdrs = {
    "g8-commands.h",
  };
  deps = {
    "@cli11//:cli11",
    ":g8-impl",
    "//c8:process",
    "//c8/stats:scope-timer",
  };
}
cc_end(0xa5b56bff);

#include "apps/g8/g8-commands.h"
#include "apps/g8/g8-impl.h"
#include "c8/process.h"
#include "c8/stats/scope-timer.h"
#include "c8/vector.h"

using namespace c8;

CLI::App_p keepHouseCmd(MainContext &ctx) {
  CLI::App_p cmd = std::make_unique<CLI::App>("Perform house keeping.", "keep-house");
  cmd->callback([&] {
    ScopeTimer t("g8");
    keepHouseImpl(ctx);
  });

  return cmd;
}

CLI::App_p inspectCmd(MainContext &ctx) {
  CLI::App_p cmd = std::make_unique<CLI::App>("Inspect files", "inspect");
  static String inspectPoint;
  static String inspectRegex;
  cmd->add_option(
    "change point", inspectPoint, "Point to look for files in (commit, branch, etc.)");
  cmd->add_option("inspect regex", inspectRegex, "Regex of files to inspect");

  cmd->callback([&] {
    ScopeTimer t("g8");
    inspectImpl(ctx, inspectPoint, inspectRegex);
  });

  return cmd;
}

CLI::App_p upgradeCmd() {
  CLI::App_p cmd = std::make_unique<CLI::App>("Upgrade g8", "upgrade");
  cmd->callback([] {
    process::ExecuteOptions opts;
    opts.file = "brew";
    opts.env = {
      "HOMEBREW_NO_AUTO_UPDATE=1",
      "HOMEBREW_NO_INSTALL_CLEANUP=1",
    };
    opts.redirectStderr = true;
    process::execute(opts, {"update"});
    process::execute(opts, {"upgrade", "g8"});
  });
  return cmd;
}

CLI::App_p cloneCmd(MainContext &ctx) {
  CLI::App_p cmd = std::make_unique<CLI::App>("Clone a remote repository", "clone");

  static String name;
  cmd->add_option("name", name, "https clone url")->required();

  static String dir;
  cmd->add_option("path", dir, "local directory path to clone into");

  static String cloneCheckoutId;
  cmd->add_option("-c,--commit", cloneCheckoutId, "After clone, checkout master to this commit");

  cmd->callback([&] {
    ScopeTimer t("g8");
    if (name.rfind("https://", 0) != 0) {
      c8::C8Log("Clone url must start with 'https://");
      exit(1);
    }
    const String host = name.substr(8, name.find('/', 9));
    if (dir.empty()) {
      auto first = name.rfind('/') + 1;
      auto last = String::npos;
      auto dot = name.rfind(".git");
      if (dot == name.size() - 4) {
        last = dot - first;
      }
      dir = name.substr(first, last);
    }
    cloneImpl(ctx, name, host, dir, cloneCheckoutId);
  });

  return cmd;
}

CLI::App_p newClientCmd(MainContext &ctx) {
  CLI::App_p cmd = std::make_unique<CLI::App>("Create new client", "newclient");

  static Vector<String> clientNames;
  cmd->add_option("client", clientNames, "Client names")->required();

  cmd->callback([&] {
    ScopeTimer t("g8");
    newClientImpl(ctx, clientNames);
  });

  return cmd;
}

CLI::App_p presubmitCmd(MainContext &ctx) {
  CLI::App_p cmd = std::make_unique<CLI::App>("Perform presubmit", "presubmit");

  static String changesetId;
  cmd->add_option("changesetId", changesetId, "changeset Id");

  cmd->callback([&] {
    ScopeTimer t("g8");
    presubmitImpl(ctx, changesetId);
  });
  return cmd;
}

CLI::App_p syncMasterCmd(MainContext &ctx) {
  CLI::App_p cmd =
    std::make_unique<CLI::App>("Fetch latest main without updating client", "sync-main");
  cmd->callback([&] {
    ScopeTimer t("g8");
    syncMasterImpl(ctx);
  });
  return cmd;
}

// New command for ls, that works just like shell 'ls' in every way but also shows files that are sparse no-checkout files not in the working directory but in the git index
CLI::App_p lsCmd(MainContext &ctx) {
  CLI::App_p cmd = std::make_unique<CLI::App>("List files", "ls");

  static Vector<String> files;
  static LsOptions opts;
  cmd->add_option("file", files, "Files");

  cmd->set_help_flag("--help", "Print this help message and exit");
  cmd->add_flag("-l", opts.longFormat, "List files in a long listing format");
  cmd->add_flag("-a", opts.all, "Include directory entries whose names begin with a dot (‘.’)");
  cmd->add_flag("-h", opts.humanReadable, "With -l, print sizes in human readable format (e.g., 1K 234M 2G)");
  cmd->add_flag("-1", opts.onePerLine, "Force output to be one entry per line");
  cmd->add_flag("-r", opts.reverse, "Reverse order while sorting");
  auto sortTime = cmd->add_flag("-t", opts.sortByTime, "Sort by modification time, newest first");
  auto sortSize = cmd->add_flag("-S", opts.sortBySize, "sort by file size, largest first");

  sortTime->excludes(sortSize);

  cmd->callback([&] {
    try {
      ScopeTimer t("g8");
      lsImpl(ctx, opts, files);
    } catch (const std::exception &e) {
      // Rethrow as CLI::Error to exit with non-zero status and error message.
      fprintf(stderr, "%s\n", e.what());
      throw CLI::RuntimeError();
    }
  });

  return cmd;
}

