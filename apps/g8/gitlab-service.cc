// Copyright (c) 2022 8th Wall, LLC.
// Original Author: Pawel Czarnecki (pawel@nianticlabs.com)

#include "bzl/inliner/rules2.h"

cc_library {
  hdrs = {
    "gitlab-service.h",
  };
  deps = {
    ":git-service-interface",
    "//c8:string",
    "//c8/string:strcat",
    "//c8/string:format",
    "//c8/network:fetch",
    "//c8:exceptions",
    "//c8:c8-log",
    "//c8:map",
    "//c8:vector",
    "@json//:json",
  };
}
cc_end(0x276f037c);

#include <algorithm>
#include <cctype>
#include <chrono>
#include <regex>
#include <thread>

#include "apps/g8/gitlab-service.h"
#include "c8/c8-log.h"
#include "c8/exceptions.h"
#include "c8/network/fetch.h"
#include "c8/string/format.h"
#include "c8/string/strcat.h"

using namespace std::chrono_literals;

namespace c8 {

namespace {

const String percentEncode(const String &src) {
  String res;
  for (const auto c : src) {
    // https://datatracker.ietf.org/doc/html/rfc3986#section-2.3
    if (isalnum(c) || c == '-' || c == '.' || c == '_' || c == '~') {
      res.push_back(c);
    } else {
      res += format("%%%X", c);
    }
  }
  return res;
}

PullRequestInfo apiMrToMrInfo(const nlohmann::json &apiMr) {
  PullRequestInfo pullInfo;
  pullInfo.pullRequestId = std::to_string(apiMr["iid"].get<int>());
  pullInfo.branchName = apiMr["source_branch"];
  pullInfo.url = apiMr["web_url"];
  pullInfo.isDraft = apiMr["draft"];
  pullInfo.isMerged = apiMr["state"] == "merged";

  // Remote heads have the format user-client-CS213985
  const String validRemoteRef = "^[a-zA-Z0-9]{1,}-[a-z0-9]{1,}-CS[0-9]{6}$";
  const std::regex validRemoteRefRegex(validRemoteRef);
  if (std::regex_match(pullInfo.branchName, validRemoteRefRegex)) {
    pullInfo.isValidG8RemoteChangesetBranch = true;
    auto dash = pullInfo.branchName.find("-");
    pullInfo.userName = pullInfo.branchName.substr(0, dash);
    // Search for second dash starting beyond the first dash.
    auto dash2 = pullInfo.branchName.find("-", ++dash);
    pullInfo.clientName = pullInfo.branchName.substr(dash, dash2 - dash);
    // Skip past the dash and CS prefix.
    pullInfo.changesetId = pullInfo.branchName.substr(dash2 + 3);
  }

  return pullInfo;
}

}  // namespace

#define VerboseC8Log(...)      \
  if (creationArgs_.verbose) { \
    C8Log(__VA_ARGS__);        \
  }

GitlabApiResult GitlabService::gitlabGet(const String &url) {
  GitlabApiResult result;
  VerboseC8Log("Send Gitlab request: GET %s", url.c_str());
  Fetch::Get(url, getHeaders())
    .then([this, &result](const FetchResponse &r) {
      result.code = r.status;
      VerboseC8Log("Received Gitlab reply: %hu %s", r.status, r.data);
      result.body = nlohmann::json::parse(r.data);
    })
    .error([&url](const c8::FetchResponse &) { C8_THROW("Failed to GET %s", url.c_str()); });
  return result;
}

GitlabApiResult GitlabService::gitlabPost(const String &url, const nlohmann::json &data) {
  GitlabApiResult result;
  VerboseC8Log("Send Gitlab request: POST %s %s", url.c_str(), data.dump().c_str());
  Fetch::Post(url, getHeaders(), data.dump())
    .then([this, &result](const FetchResponse &r) {
      result.code = r.status;
      VerboseC8Log("Received Gitlab reply: %hu %s", r.status, r.data);
      result.body = nlohmann::json::parse(r.data);
    })
    .error([&url](const c8::FetchResponse &) { C8_THROW("Failed to POST %s", url.c_str()); });
  return result;
}

GitlabApiResult GitlabService::gitlabPut(const String &url, const nlohmann::json &data) {
  GitlabApiResult result;
  VerboseC8Log("Send Gitlab request: PUT %s %s", url.c_str(), data.dump().c_str());
  Fetch::Put(url, getHeaders(), data.dump())
    .then([this, &result](const FetchResponse &r) {
      result.code = r.status;
      VerboseC8Log("Received Gitlab reply: %hu %s", r.status, r.data);
      result.body = nlohmann::json::parse(r.data);
    })
    .error([&url](const c8::FetchResponse &) { C8_THROW("Failed to PUT %s", url.c_str()); });
  return result;
}

String GitlabService::getCreateMrUrl() {
  return strCat(
    "https://",
    creationArgs_.endpoint,
    "/api/v4/projects/",
    percentEncode(creationArgs_.repoName),
    "/merge_requests");
}

String GitlabService::getMrByIdUrl(const String &mrNum) {
  return strCat(
    "https://",
    creationArgs_.endpoint,
    "/api/v4/projects/",
    percentEncode(creationArgs_.repoName),
    "/merge_requests/",
    percentEncode(mrNum));
}

String GitlabService::getMergeTrainByMrIdUrl(const String &mrNum) {
  return strCat(
    "https://",
    creationArgs_.endpoint,
    "/api/v4/projects/",
    percentEncode(creationArgs_.repoName),
    "/merge_trains/merge_requests/",
    percentEncode(mrNum));
}

String GitlabService::getLandMrUrl(const String &mrNum) {
  return strCat(getMrByIdUrl(mrNum), "/merge");
}

String GitlabService::getPostNotesUrl(const String &mrNum) {
  return strCat(getMrByIdUrl(mrNum), "/notes");
}

String GitlabService::getRebaseMrUrl(const String &mrNum) {
  return strCat(getMrByIdUrl(mrNum), "/rebase");
}

String GitlabService::getMrByBranchUrl(const String &branch) {
  return strCat(
    "https://",
    creationArgs_.endpoint,
    "/api/v4/projects/",
    percentEncode(creationArgs_.repoName),
    "/merge_requests?source_branch=",
    percentEncode(branch),
    "&with_merge_status_recheck=true");
}

String GitlabService::getUserUrl(const String &user) {
  return strCat("https://", creationArgs_.endpoint, "/api/v4/users?username=", percentEncode(user));
}

String GitlabService::getCurrentlyAuthenticatedUser() {
  return strCat("https://", creationArgs_.endpoint, "/api/v4/user");
}

std::optional<CreatePullRequestResult> GitlabService::createPullRequest(
  const CreatePullRequestArgs &args) {
  auto currentlyAuthenticatedUser = gitlabGet(getCurrentlyAuthenticatedUser());
  auto apiReply = gitlabPost(
    getCreateMrUrl(),
    {
      {"source_branch", args.headRef},
      {"target_branch", creationArgs_.mainBranchName},
      {"assignee_id", currentlyAuthenticatedUser.body["id"]},
      {"title", args.draft ? strCat("Draft: ", args.title) : args.title},
      {"description", args.body},
      {"remove_source_branch", true},
      {"squash", true},
    });

  CreatePullRequestResult result;
  if (!apiReply.body.contains("iid")) {
    C8Log("Failed to create MR. Reply: %s", apiReply.body.dump().c_str());
    return std::nullopt;
  }
  // iid is the MR number. id is the project id.
  result.pullRequestId = std::to_string(apiReply.body["iid"].get<int>());
  return result;
}

// NOTE(pawel) MR must not be in draft state else it will fail with "405 Method Not Allowed".
// Make sure to setPullRequestDraftStatus() to mark it ready before trying to land.
std::optional<LandRequestResult> GitlabService::landPullRequest(const LandRequestArgs &args) {
  // PUT /projects/:id/merge_requests/:merge_request_iid/merge
  if (args.withCi) {
    return landPullRequestWithCi(args);
  }

  nlohmann::json mergeRequestBody = {
    {"squash", true},
    {"sha", args.expectedTipSha},
    {"squash_commit_message", args.commitTitle},
    {"should_remove_source_branch", true},
    {"merge_when_pipeline_succeeds", false},
  };

  auto apiReply = gitlabPut(getLandMrUrl(args.pullRequestId), mergeRequestBody);

  if (apiReply.code == 422) {
    C8Log("Trying to rebase...");
    auto rebaseReply = gitlabPut(
      getRebaseMrUrl(args.pullRequestId),
      {
        {"skip_ci", true},
      });
    if (rebaseReply.code != 202) {
      C8Log("MR cannot be rebased automatically. You may need to sync.");
      return std::nullopt;
    }

    // It looks like gitlab may have a short lived cache or have eventually consistent workings
    // so we give it a quick sec to settle.
    std::this_thread::sleep_for(2s);
    apiReply = gitlabPut(
      getLandMrUrl(args.pullRequestId),
      {
        {"squash", true},
        // NOTE(pawel) Leaving out the expected tip sha because gitlab produced the rebase
        // commit that we don't have locally.
        {"squash_commit_message", args.commitTitle},
        {"should_remove_source_branch", true},
        {"merge_when_pipeline_succeeds", false},
      });
  }

  if (apiReply.code == 405) {
    C8Log("Landing not allowed (405). Do you have the required approvals?");
    return std::nullopt;
  }

  if (apiReply.code == 422) {
    C8Log("Landing failed (422). Try g8 sync && g8 update before landing.");
    return std::nullopt;
  }

  if (!apiReply.body.contains("state") || apiReply.body["state"] != "merged") {
    C8Log("Failed to merge MR. status=%hu, msg=%s", apiReply.code, apiReply.body.dump().c_str());
    return std::nullopt;
  }

  // NOTE(pawel) If a merge commit happened that will be later in the main branch history so
  // we want to rebase on top of it otherwise it appears that the sha value returns what was
  // landed.
  LandRequestResult result;
  if (!apiReply.body["merge_commit_sha"].is_null()) {
    result.landedMasterSha = apiReply.body["merge_commit_sha"];
  } else if (!apiReply.body["squash_commit_sha"].is_null()) {
    result.landedMasterSha = apiReply.body["squash_commit_sha"];
  } else {
    C8Log("Warning: Gitlab land returned null for both merge_commit_sha and squash_commit_sha");
  }
  return result;
}

std::optional<PullRequestInfo> GitlabService::pullRequestById(const String &mergeRequestId) {
  auto apiReply = gitlabGet(getMrByIdUrl(mergeRequestId));

  if (!apiReply.body.contains("iid")) {
    C8Log("Gitlab MR %s not found", mergeRequestId.c_str());
    return std::nullopt;
  }

  return apiMrToMrInfo(apiReply.body);
}

std::optional<PullRequestInfo> GitlabService::pullRequestByBranch(const String &branchName) {
  auto apiReply = gitlabGet(getMrByBranchUrl(branchName));

  if (!apiReply.body.is_array() || apiReply.body.size() < 1 || !apiReply.body[0].contains("iid")) {
    C8Log("Gitlab MR not found: %s", apiReply.body.dump().c_str());
    return std::nullopt;
  }

  return apiMrToMrInfo(apiReply.body[0]);
}

std::optional<LandRequestResult> GitlabService::landPullRequestWithCi(const LandRequestArgs &args) {
  auto mergeReply = gitlabPost(
    getMergeTrainByMrIdUrl(args.pullRequestId),
    {
      {"squash", true},
      {"sha", args.expectedTipSha},
    });

  if (mergeReply.code == 422) {
    C8Log("Trying to rebase...");
    auto rebaseReply = gitlabPut(getRebaseMrUrl(args.pullRequestId), {});
    if (rebaseReply.code != 202) {
      C8Log("MR cannot be rebased automatically. You may need to sync.");
      return std::nullopt;
    }

    // It looks like gitlab may have a short lived cache or have eventually consistent workings
    // so we give it a quick sec to settle.
    std::this_thread::sleep_for(2s);
    mergeReply = gitlabPost(
      getMergeTrainByMrIdUrl(args.pullRequestId),
      {
        {"squash", true},
      });
  }

  if (mergeReply.code == 201) {
    return LandRequestResult{};
  }

  // NOTE(christoph): 400 is returned when merge trains are not enabled, fall back to
  // normal "merge after pipeline success".
  auto shouldFallbackToPipeline = mergeReply.code == 400;

  if (shouldFallbackToPipeline) {
    mergeReply = gitlabPut(
      getLandMrUrl(args.pullRequestId),
      {
        {"squash", true},
        {"sha", args.expectedTipSha},
        {"squash_commit_message", args.commitTitle},
        {"should_remove_source_branch", true},
        {"merge_when_pipeline_succeeds", true},
      });

    if (mergeReply.code == 422) {
      C8Log("Trying to rebase...");

      auto rebaseReply = gitlabPut(getRebaseMrUrl(args.pullRequestId), {});
      if (rebaseReply.code != 202) {
        C8Log("MR cannot be rebased automatically. You may need to sync.");
        return std::nullopt;
      }

      // It looks like gitlab may have a short lived cache or have eventually consistent workings
      // so we give it a quick sec to settle.
      std::this_thread::sleep_for(2s);

      mergeReply = gitlabPut(
        getLandMrUrl(args.pullRequestId),
        {
          {"squash", true},
          {"squash_commit_message", args.commitTitle},
          {"should_remove_source_branch", true},
          {"merge_when_pipeline_succeeds", true},
        });
    }

    if (mergeReply.code == 200) {
      return LandRequestResult{};
    }
  }

  if (mergeReply.code == 405) {
    C8Log("Landing not allowed (405). Is the MR ready to merge?");
  } else {
    C8Log("Land failed (%d).", mergeReply.code);
  }

  return std::nullopt;
}

bool GitlabService::setPullRequestDraftStatus(const DraftStatusRequest &args) {
  // Gitlab draft status is based on MR title. If it begins with "draft" then it is draft.
  // We first get the mr and if the draft status does not agree with our requested status
  // we can comment "/draft" which is a gitlab quick action that will toggle the draft status.
  // It doesn't look like there is a totally foolproof way to "make ready" via the api...

  auto mr = pullRequestById(args.pullRequestId);
  if (!mr) {
    return false;
  }

  if (mr->isDraft == args.shouldBeDraft) {
    return true;
  }

  // POST /projects/:id/merge_requests/:merge_request_iid/notes
  auto apiReply = gitlabPost(getPostNotesUrl(args.pullRequestId), {{"body", "/draft"}});

  // TODO(pawel) Confirm that response was successful?
  // Sample reply to mark ready.
  // {"commands_changes":{"wip_event":"unwip"},"summary":["Unmarked this merge request as a
  // draft."]}
  return true;
}

bool GitlabService::addReviewers(const ReviewersRequest &args) {
  if (args.reviewers.empty()) {
    return true;
  }
  Vector<int> reviewers;

  // Get the existing reviewers assigned to the MR.
  auto mrReply = gitlabGet(getMrByIdUrl(args.pullRequestId));
  if (!mrReply.body.contains("iid")) {
    C8Log("Gitlab MR %s not found", args.pullRequestId.c_str());
    return false;
  }
  for (const auto &existingRemoteReviewer : mrReply.body["reviewers"]) {
    reviewers.push_back(existingRemoteReviewer["id"]);
  }

  // Get the ID's of the new reviewers that will be added to the MR.
  for (const auto &reviewerUsername : args.reviewers) {
    auto reviewerReply = gitlabGet(getUserUrl(reviewerUsername));
    if (
      !reviewerReply.body.is_array() || reviewerReply.body.size() < 1
      || !reviewerReply.body[0].contains("id")) {
      C8Log("Gitlab reviewer not found: %s", reviewerUsername.c_str());
      return false;
    }
    int newReviewerId = reviewerReply.body[0]["id"];
    if (auto r = std::find(begin(reviewers), end(reviewers), newReviewerId); r == end(reviewers)) {
      reviewers.push_back(newReviewerId);
    }
  }

  // Update the MR with the new reviewers.
  auto addReviewersReply =
    gitlabPut(getMrByIdUrl(args.pullRequestId), {{"reviewer_ids", reviewers}});

  return true;
}

bool GitlabService::updateTitle(const TitleRequest &args) {
  auto updateTitleReply = gitlabPut(getMrByIdUrl(args.pullRequestId), {{"title", args.message}});
  if (updateTitleReply.code != 200) {
    return false;
  }
  return true;
}
}  // namespace c8
