// Copyright (c) 2022 8th Wall, LLC.
// Author: Pawel Czarnecki (pawel@nianticlabs.com)

#include "bzl/inliner/rules2.h"

cc_binary {
  deps = {
    ":g8-config",
    ":g8-old",
    ":g8-commands",
    ":g8-helpers",
    "@cli11//:cli11",
    "@curl//:curl",
    "//c8:c8-log",
    "//c8:vector",
    "//c8:map",
    "//c8/stats:scope-timer",
    "//c8/stats:export-detail",
  };
}
cc_end(0xe523a729);

#include <sys/resource.h>

#include "apps/g8/g8-commands.h"
#include "apps/g8/g8-config.h"
#include "apps/g8/g8-helpers.h"
#include "apps/g8/g8-old.h"
#include "c8/c8-log.h"
#include "c8/map.h"
#include "c8/stats/export-detail.h"
#include "c8/stats/scope-timer.h"
#include "c8/vector.h"
#include "curl/curl.h"

using namespace c8;

// Requests all the nice things.
// RLIMIT_NOFILE is the limit on the number of file descriptors we can have open.
// Sometimes we need more than the macOS default of 256 (on my computer it was 12800) so we increase
// our limit to the system's hard limit.
// https://pubs.opengroup.org/onlinepubs/9699919799/
void ensureExecutionEnvironment() {
  static std::once_flag onceFlag;
  std::call_once(onceFlag, []() {
    struct rlimit limits;

    if (getrlimit(RLIMIT_NOFILE, &limits) != 0) {
      C8Log("getrlimit returned %d", errno);
      return;
    }

    // Allow ourselves to create as many files as we possibly can.
    limits.rlim_cur = limits.rlim_max;

    if (setrlimit(RLIMIT_NOFILE, &limits) != 0) {
      C8Log("setrlimit returned %d", errno);
      return;
    }
  });
}

// Used for CURLOPT_WRITEFUNCTION when checking host connectivity.
size_t noopWrite(char *, size_t size, size_t nmemb, void *) { return size * nmemb; }

CLI::App_p setCommonOptions(CLI::App_p app) {
  app->fallthrough();
  return app;
}

int mainInternal(MainContext &context, int argc, char *argv[]) {
  context.repoPath = repoPathOrEmpty();
  ensureExecutionEnvironment();

  CLI::App app{"g8 - 8th Wall Source Control"};
  app.add_flag("-v", context.verbose, "verbose");
  app.set_version_flag("--version", G8_VERSION, "Print g8 version and exit");

  Vector<String> requiresNetwork = {
    "add",
    "client",
    "clone",
    "change",
    "fetch-remote-clients",
    "files",
    "initial-commit",
    "land",
    "newchange",
    "save",
    "send",
    "sync",
    "unadd",
    "update",
    "mr",
    "pr",
    "keep-house",
  };

  // We want to do special checks for certain hosts. The niantic gitlab, for example, is behind a
  // vpn and sometimes it happens that a user runs g8 without the vpn connection active. Here we
  // can make such checks to convince ourselves that the network operations will probably work.
  HashMap<String, std::function<std::optional<String>()>> checksForHost = {
    {"gitlab.<REMOVED_BEFORE_OPEN_SOURCING>.com",
     []() -> std::optional<String> {
       CURL *curl = curl_easy_init();
       curl_easy_setopt(curl, CURLOPT_URL, "https://gitlab.<REMOVED_BEFORE_OPEN_SOURCING>.com");
       curl_easy_setopt(curl, CURLOPT_TIMEOUT, 3L);
       curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, noopWrite);
       curl_easy_setopt(curl, CURLOPT_WRITEDATA, nullptr);
       CURLcode res = curl_easy_perform(curl);
       curl_easy_cleanup(curl);
       if (res != CURLE_OK) {
         return "Timeout connecting to gitlab. Are you connected to VPN?";
       }
       return std::nullopt;
     }},
  };

  if (argc >= 2) {
    if (std::find(begin(requiresNetwork), end(requiresNetwork), argv[1]) != end(requiresNetwork)) {
      auto checkFunc = checksForHost[repoHost(context.repoPath)];
      if (checkFunc) {
        auto error = checkFunc();
        if (error) {
          C8Log(error->c_str());
          exit(1);
        }
      }
    }
  }

  // Add subcommands.
  app.add_subcommand(setCommonOptions(addCmd(context)));
  app.add_subcommand(setCommonOptions(clientCmd(context)));
  app.add_subcommand(setCommonOptions(cloneCmd(context)));
  app.add_subcommand(setCommonOptions(changeCmd(context)));
  app.add_subcommand(setCommonOptions(diffCmd(context)));
  app.add_subcommand(setCommonOptions(drySyncCmd(context)));
  app.add_subcommand(setCommonOptions(fetchRemoteClientsCmd(context)));
  app.add_subcommand(setCommonOptions(filesCmd(context)));
  app.add_subcommand(setCommonOptions(initialCommitCmd(context)));
  app.add_subcommand(setCommonOptions(infoCmd(context)));
  app.add_subcommand(setCommonOptions(landCmd(context)));
  app.add_subcommand(setCommonOptions(logCmd(context)));
  app.add_subcommand(setCommonOptions(newchangeCmd(context)));
  app.add_subcommand(setCommonOptions(revertCmd(context)));
  app.add_subcommand(setCommonOptions(saveCmd(context)));
  app.add_subcommand(setCommonOptions(sendCmd(context)));
  app.add_subcommand(setCommonOptions(statusCmd(context)));
  app.add_subcommand(setCommonOptions(syncCmd(context)));
  app.add_subcommand(setCommonOptions(unaddCmd(context)));
  app.add_subcommand(setCommonOptions(updateCmd(context)));
  app.add_subcommand(setCommonOptions(patchCmd(context)));
  app.add_subcommand(setCommonOptions(prCmd(context)));
  app.add_subcommand(setCommonOptions(mrCmd(context)));
  app.add_subcommand(setCommonOptions(keepHouseCmd(context)));
  app.add_subcommand(setCommonOptions(inspectCmd(context)));
  app.add_subcommand(setCommonOptions(presubmitCmd(context)));
  app.add_subcommand(setCommonOptions(upgradeCmd()));
  app.add_subcommand(setCommonOptions(syncMasterCmd(context)));
  app.add_subcommand(setCommonOptions(lsCmd(context)));

  // DEPRECATED
  app.add_subcommand(newClientCmd(context));  // prefer g8 client [new client name]

  app.require_subcommand(1, 1);

  CLI11_PARSE(app, argc, argv);

  return 0;
}

inline void replaceChar(String &haystack, char target, char replacement) {
  for (char &c : haystack) {
    if (c == target) {
      c = replacement;
    }
  }
}

int main(int argc, char *argv[]) {
  MainContext context;
  int result = mainInternal(context, argc, argv);

  if (context.verbose) {
    auto loggingContext = ScopeTimer::lastCompleted();
    MutableRootMessage<LoggingDetail> detailRoot;
    auto detailBuilder = detailRoot.builder();
    loggingContext->exportDetail(&detailBuilder);

    auto adjustedDurations = computeFlamegraphValues(detailRoot.reader());

    for (auto detail : detailRoot.reader().getEvents()) {
      String line(&detail.getEventName().cStr()[1]);  // ub if event name is empty.
      replaceChar(line, '/', ';');
      line += " ";
      line += std::to_string(adjustedDurations[detail.getEventId().cStr()]);
      C8Log(line.c_str());
    }
  }

  return result;
}
