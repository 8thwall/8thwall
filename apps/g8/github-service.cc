// Copyright (c) 2022 8th Wall, LLC.
// Original Author: Pawel Czarnecki (pawel@nianticlabs.com)

#include "bzl/inliner/rules2.h"

cc_library {
  hdrs = {
    "github-service.h",
  };
  deps = {
    ":git-service-interface",
    "//c8:string",
    "//c8/string:strcat",
    "//c8:c8-log",
    "//c8:map",
    "//c8:process",
    "//c8:vector",
    "@json//:json",
  };
}
cc_end(0xd8e18820);

#include <regex>

#include "apps/g8/github-service.h"
#include "c8/c8-log.h"
#include "c8/process.h"
#include "c8/string/strcat.h"

#define VerboseC8Log(...)      \
  if (creationArgs_.verbose) { \
    C8Log(__VA_ARGS__);        \
  }

#define CurlGithubReturnNullopt(name, ...)   \
  nlohmann::json name;                       \
  {                                          \
    auto __##name = curlGithub(__VA_ARGS__); \
    if (!__##name) {                         \
      return std::nullopt;                   \
    }                                        \
    name = *__##name;                        \
  }

#define CurlGithubReturnFalse(name, ...)     \
  nlohmann::json name;                       \
  {                                          \
    auto __##name = curlGithub(__VA_ARGS__); \
    if (!__##name) {                         \
      return false;                          \
    }                                        \
    name = *__##name;                        \
  }

namespace c8 {

template <typename... Args>
std::optional<nlohmann::json> GithubService::curlGithub(Args &&...args) {
  auto stdout = process::execute(
    "curl",
    "-s",
    "-H",
    "Authorization: token " + creationArgs_.credential,
    "-H",
    "Content-Type: application/json",
    "-H",
    "Accept: application/vnd.github.shadow-cat-preview+json",
    args...);
  auto rawReply = process::getOutput(stdout);
  try {
    return nlohmann::json::parse(rawReply);
  } catch (nlohmann::json::exception &e) {
    C8Log("Failed to parse gitlab response %s", rawReply.c_str());
    return std::nullopt;
  }
}

// Returning PR number indicates PR was created successfully.
std::optional<CreatePullRequestResult> GithubService::createPullRequest(
  const CreatePullRequestArgs &args) {
  auto reqUrl = strCat("https://api.github.com/repos/", creationArgs_.repoName, "/pulls");
  nlohmann::json req;
  req["title"] = args.title;
  req["body"] = args.body;
  req["head"] = args.headRef;
  req["base"] = creationArgs_.mainBranchName;
  req["draft"] = args.draft;

  VerboseC8Log("Sending Github request to %s:\n%s", reqUrl.c_str(), req.dump().c_str());
  CurlGithubReturnNullopt(apiReply, "-X", "POST", "-d", req.dump(), reqUrl);
  VerboseC8Log("Received Github reply:\n%s", apiReply.dump().c_str());

  CreatePullRequestResult result;
  result.pullRequestId = std::to_string(apiReply["number"].get<int>());
  return result;
}

// Retuning the new master sha indicates that land succeeded.
std::optional<LandRequestResult> GithubService::landPullRequest(const LandRequestArgs &args) {
  if (args.withCi) {
    C8Log("withCi is not implemented for Github repos");
    return std::nullopt;
  }

  nlohmann::json githubRequest{
    {"merge_method", "squash"},
    {"sha", args.expectedTipSha},
    {"commit_title", args.commitTitle},
    {"commit_message", args.commitMessage},
  };

  auto reqUrl = strCat(
    "https://api.github.com/repos/",
    creationArgs_.repoName,
    "/pulls/",
    args.pullRequestId,
    "/merge");

  VerboseC8Log("[Sending GitHub Request]:\n%s\n%s", reqUrl.c_str(), githubRequest.dump().c_str());
  CurlGithubReturnNullopt(githubResponse, "-X", "PUT", "-d", githubRequest.dump(), reqUrl);
  VerboseC8Log(
    "[Received GitHub Response]:\n%s\n%s", reqUrl.c_str(), githubResponse.dump().c_str());

  if (!githubResponse.contains("merged") || githubResponse["merged"] != true) {
    // TODO(pawel) Can we give a reason why it didn't work out?
    return std::nullopt;
  }

  LandRequestResult ret;
  ret.landedMasterSha = githubResponse["sha"];

  return ret;
}

std::optional<PullRequestInfo> GithubService::pullRequestById(const String &pullId) {
  auto reqUrl = strCat("https://api.github.com/repos/", creationArgs_.repoName, "/pulls/", pullId);
  VerboseC8Log("Sending Github request: %s", reqUrl.c_str());
  CurlGithubReturnNullopt(githubResponse, reqUrl);
  VerboseC8Log("Received Github response: %s", githubResponse.dump().c_str());

  if (!githubResponse.contains("head")) {
    return std::nullopt;
  }

  return pullRequestByBranch(githubResponse["head"]["ref"]);
}

std::optional<PullRequestInfo> GithubService::pullRequestByBranch(const String &branchName) {
  auto remoteName = creationArgs_.repoName;

  auto owner = remoteName.substr(0, remoteName.find_first_of("/"));
  auto repo = remoteName.substr(remoteName.find_first_of("/") + 1);

  String reqUrl = "https://api.github.com/graphql";
  String query = "{\"query\": \"{repository(owner:\\\"" + owner + "\\\", name:\\\"" + repo + "\\\"){"
    "pullRequests(headRefName:\\\"" + branchName + "\\\", first: 1){totalCount, nodes {number, id, isDraft, permalink, state}}}}\"}";

  VerboseC8Log("Sending Github request to %s:\n%s", reqUrl.c_str(), query.c_str());
  CurlGithubReturnNullopt(apiReply, "-d", query, "-X", "POST", reqUrl);
  VerboseC8Log("Received Github reply:\n%s", apiReply.dump().c_str());

  if (!apiReply.count("data")) {
    C8Log("No data from github when querying pull request: %s", apiReply.dump().c_str());
    return std::nullopt;
  }

  if (apiReply["data"]["repository"]["pullRequests"]["nodes"].empty()) {
    C8Log("Github PR not found for branchName %s.", branchName.c_str());
    return std::nullopt;
  }

  auto node = apiReply["data"]["repository"]["pullRequests"]["nodes"][0];

  PullRequestInfo pullInfo;
  pullInfo.branchName = branchName;
  pullInfo.pullRequestId = std::to_string(node["number"].get<int>());
  pullInfo.url = node["permalink"];
  pullInfo.isDraft = node["isDraft"];
  pullInfo.isMerged = node["state"] == "MERGED";

  // Remote heads have the format
  // user-client-CS213985
  const String validRemoteRef = "^[a-zA-Z0-9]{1,}-[a-z0-9]{1,}-CS[0-9]{6}$";
  const std::regex validRemoteRefRegex(validRemoteRef);
  if (std::regex_match(branchName, validRemoteRefRegex)) {
    pullInfo.isValidG8RemoteChangesetBranch = true;
    auto dash = branchName.find("-");
    pullInfo.userName = branchName.substr(0, dash);

    // Search for second dash starting beyond the first dash.
    auto dash2 = branchName.find("-", ++dash);
    pullInfo.clientName = branchName.substr(dash, dash2 - dash);

    pullInfo.changesetId = branchName.substr(dash2 + 3);  // Skip past the dash and CS prefix.
  }

  // NOTE(pawel) To mark draft status for github we need graphql id.
  prNumberToGraphqlId_[pullInfo.pullRequestId] = node["id"];

  return pullInfo;
}

bool GithubService::setPullRequestDraftStatus(const DraftStatusRequest &arg) {
  if (prNumberToGraphqlId_.find(arg.pullRequestId) == prNumberToGraphqlId_.end()) {
    // Need to fetch pr and cache the graphql id.
    pullRequestById(arg.pullRequestId);
  }

  if (prNumberToGraphqlId_.find(arg.pullRequestId) == prNumberToGraphqlId_.end()) {
    C8Log("Unable to resolve github pr to a graphql node %s", arg.pullRequestId.c_str());
    return false;
  }

  auto graphqlId = prNumberToGraphqlId_[arg.pullRequestId];
  String reqUrl = "https://api.github.com/graphql";
  String query = "{\"query\":\"mutation {markPullRequestReadyForReview(input:{pullRequestId:\\\""
    + graphqlId + "\\\"}){pullRequest{id}}}\"}";

  CurlGithubReturnFalse(apiReply, "-X", "POST", "-d", query, reqUrl);

  if (apiReply.contains("errors")) {
    C8Log(
      "ERROR: could not mark PR ready for review... github response:\n%s", apiReply.dump().c_str());
    return false;
  }
  return true;
}

bool GithubService::addReviewers(const ReviewersRequest &arg) {
  if (arg.reviewers.empty()) {
    return true;
  }
  auto reqUrl = strCat(
    "https://api.github.com/repos/",
    creationArgs_.repoName,
    "/pulls/",
    arg.pullRequestId,
    "/requested_reviewers");
  nlohmann::json apiRequest;
  apiRequest["reviewers"] = arg.reviewers;
  VerboseC8Log("Github request reviewers:\n%s", apiRequest.dump().c_str());
  CurlGithubReturnFalse(reply, "-X", "POST", "-d", apiRequest.dump(), reqUrl);
  VerboseC8Log("Github request for reviewers reply:\n%s", reply.dump().c_str());
  // TODO(pawel) Detect failure and return false.
  return true;
}

bool GithubService::updateTitle(const TitleRequest &arg) {
  C8Log("Info: Not Updating Github PR Title");
  return true;
}
}  // namespace c8
