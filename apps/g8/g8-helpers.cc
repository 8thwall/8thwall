// Copyright (c) 2022 8th Wall, LLC.
// Author: Pawel Czarnecki (pawel@nianticlabs.com)

#include "bzl/inliner/rules2.h"

cc_library {
  hdrs = {
    "g8-helpers.h",
  };
  deps = {
    "@cli11//:cli11",
    "//c8/io:capnp-messages",
    "//c8:c8-log",
    "//c8/string:split",
    "//c8/string:format",
    "//c8/io:file-io",
    "//c8/pixels:base64",
    "//c8:process",
    "//c8/git:g8-api.capnp-cc",
    "//c8/git:g8-git",
    "//c8/stats:scope-timer",
    "//c8/io:capnp-messages",
  };
}
cc_end(0xf3cb6c4d);

#include <filesystem>
#include <fstream>
#include <map>
#include <sstream>
#include <string>
#include <vector>

#include "apps/g8/g8-helpers.h"
#include "c8/git/g8-api.capnp.h"
#include "c8/git/g8-git.h"
#include "c8/io/file-io.h"
#include "c8/pixels/base64.h"
#include "c8/process.h"
#include "c8/stats/scope-timer.h"
#include "c8/string/format.h"
#include "c8/string/split.h"

namespace fs = std::filesystem;
using namespace c8;

String repoPath() {
  auto cwd = fs::current_path();
  auto path = cwd;
  while (path.string() != "/") {
    if (fs::exists(path / ".git")) {
      return path.c_str();
    }
    path = path.parent_path();
  }
  C8Log("%sERROR:%s \"%s\" is not in a g8 repo.", RED, CLEAR, cwd.c_str());
  throw CLI::RuntimeError(1);
}

String repoPathOrEmpty() {
  auto cwd = fs::current_path();
  auto path = cwd;
  while (path.string() != "/") {
    if (fs::exists(path / ".git")) {
      return path.c_str();
    }
    path = path.parent_path();
  }
  return "";
}

String dataString(const Vector<uint8_t> &data) {
  return {reinterpret_cast<const char *>(data.data()), data.size()};
}

double epochMicros() {
  return std::chrono::time_point_cast<std::chrono::microseconds>(std::chrono::system_clock::now())
    .time_since_epoch()
    .count();
}

void read8thWallCreds(const String &host, String *jwt, String *err) {
  *jwt = "";
  *err = "";
  String credsFile = format("%s/.auth8/credentials", std::getenv("HOME"));
  if (!std::filesystem::exists(credsFile)) {
    *err = "No 8th Wall credentials found";
    *jwt = "";
    return;
  }
  *jwt = readTextFile(credsFile);
  auto jwtParts = split(*jwt, ".");
  if (jwtParts.size() != 3) {
    *err = "Corrupt credential data";
    *jwt = "";
    return;
  }
  auto body = dataString(decode(jwtParts[1]));
  nlohmann::json jwtBody;
  try {
    jwtBody = nlohmann::json::parse(body);
  } catch (nlohmann::json::exception &e) {
    *err = "Corrupt credential data body";
    *jwt = "";
    return;
  }

  auto expMinutes = (static_cast<double>(jwtBody["exp"].get<int>()) * 1e6 - epochMicros()) / 6e7;
  if (expMinutes < 5.0) {
    *err = "Expired 8th Wall credentials";
    *jwt = "";
    return;
  }
}

String resolveConfigured(const Vector<String> &els, const String &defaultValue) {
  for (auto param : els) {
    auto val = std::getenv(param.c_str());
    if (val != nullptr && strlen(val) != 0) {
      return val;
    }
  }
  return defaultValue;
}

String resolveEditor() {
  return resolveConfigured(
    {
      "G8EDITOR",
      "EDITOR",
    },
    "vi");
}

String resolveMergeTool() {
  return resolveConfigured(
    {
      "G8MERGE",
      "EDITOR",
    },
    "vi");
}

String resolveDiffTool() {
  return resolveConfigured(
    {
      "G8DIFF",
    },
    "less -RFX");
}

bool is8wHost(const String &host) { return host.find("8thwall") != String::npos; }

const std::pair<String, String> getCredentialsFromSecureStorageForHost(const String &host) {
  auto [stdin, stdout] = process::execute2("git", "credential", "fill");
  stdin << "host=" << host << "\nprotocol=https\n\n\n";
  std::stringstream raw{process::getOutput(stdout)};

  String user, pass;
  String line, field, value;

  while (raw >> line) {
    auto delim = line.find_first_of("=");
    field = line.substr(0, delim);

    if (field == "username") {
      user = line.substr(delim + 1, line.size());
    } else if (field == "password") {
      pass = line.substr(delim + 1, line.size());
    }
  }

  if (user.empty() || pass.empty()) {
    C8Log("[warning] could not find credentials for %s", host.c_str());
  }

  return {user, pass};
}

void setCredsForHost(G8Authentication::Builder creds, const String &host) {
  if (is8wHost(host)) {
    String token;
    String tokenErr;
    // Read credentials.
    read8thWallCreds(host, &token, &tokenErr);
    if (token.empty()) {
      // If credentials are missing or invalid, run auth8.
      C8Log("%sINFO:%s %s. Fetching 8th Wall credentials.", GREEN, CLEAR, tokenErr.c_str());
      auto stdout = process::execute("auth8");
      C8Log("%sINFO:%s %s", GREEN, CLEAR, process::getOutput(stdout).c_str());
      // Read credentials again. If still missing or invalid, print an error and exit.
      read8thWallCreds(host, &token, &tokenErr);
      if (token.empty()) {
        C8Log("%ERROR:%s %s. Please run auth8.", RED, CLEAR, tokenErr.c_str());
        exit(1);
      }
    }
    creds.setCred(token.substr(0, token.size() - 1));  // trim trailing newline.
  } else {
    auto [user, pass] = getCredentialsFromSecureStorageForHost(host);
    creds.setUser(user);
    creds.setPass(pass);
  }
}

String urlHost(const String &url) {
  if (url.size() == 0) {
    return "";
  }
  auto start = url.find("//");  // find the protocol separater
  auto lessProtocol = url.substr(start == String::npos ? 0 : start + 2);
  auto end = lessProtocol.find("/");
  return lessProtocol.substr(0, end);
}

String repoHost(const String &repo) {
  if (repo.empty()) {
    return "";
  }
  MutableRootMessage<G8RepositoryRequest> infoReq;
  infoReq.builder().setRepo(repo);
  infoReq.builder().setAction(G8RepositoryRequest::Action::GET_INFO);
  auto infoRes = g8Repository(infoReq.reader());
  return infoRes.reader().getStatus().getCode() == 0 ? infoRes.reader().getRemote().getHost() : "";
}

void setCredsForCwdRepo(G8Authentication::Builder creds) {
  ScopeTimer t("get-creds");
  setCredsForHost(creds, repoHost(repoPath()));
}

std::optional<G8Changeset::Reader> extractChangeset(
  G8ChangesetResponse::Reader reader, const String &changesetId) {
  auto changesets = reader.getChangeset();
  auto changeset = std::find_if(std::begin(changesets), std::end(changesets), [&](auto cs) {
    return cs.getId() == changesetId;
  });
  if (changeset == std::end(changesets)) {
    return std::nullopt;
  }
  return *changeset;
}

bool isChangesetDirty(G8Changeset::Reader changeset) {
  auto fileListInfo = changeset.getFileList().getInfo();
  auto dirtyInfo = std::find_if(
    std::begin(fileListInfo), std::end(fileListInfo), [&](auto f) { return f.getDirty(); });
  return dirtyInfo != std::end(fileListInfo);
}

Vector<String> extractChangesetPaths(G8Changeset::Reader changeset) {
  Vector<String> modifiedFiles = {};
  for (const auto &info : changeset.getFileList().getInfo()) {
    modifiedFiles.push_back(info.getPath().cStr());
  }
  return modifiedFiles;
}

bool parseUserConfig(std::map<std::string, std::vector<std::string>> &groups) {
  const std::string configPath = repoPath() + "/.git/info/user_groups.txt";

  std::ifstream file(configPath);
  if (!file.is_open()) {
    C8Log("ERROR: Failed to open config file: %s", configPath.c_str());
    return false;
  }

  std::string line;
  while (std::getline(file, line)) {
    std::istringstream iss(line);
    std::string group, users;

    if (std::getline(iss, group, '=') && std::getline(iss, users)) {
      std::istringstream usersStream(users);
      std::string user;
      while (std::getline(usersStream, user, ',')) {
        groups[group].push_back(user);
      }
    }
  }

  return true;
}
