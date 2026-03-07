// Copyright (c) 2022 8th Wall, LLC.
// Original Author: Pawel Czarnecki (pawel@nianticlabs.com)

#pragma once

#include "apps/g8/git-service-interface.h"
#include "c8/map.h"
#include "nlohmann/json.hpp"

namespace c8 {

struct GitlabApiResult {
  uint16_t code;
  nlohmann::json body;
};

class GitlabService : public GitServiceInterface {
public:
  explicit GitlabService(GitServiceFactoryArgs args) : creationArgs_(args) {}

  virtual const GitServiceFactoryArgs &getArgs() { return creationArgs_; }

  virtual std::optional<CreatePullRequestResult> createPullRequest(const CreatePullRequestArgs &);

  virtual std::optional<LandRequestResult> landPullRequest(const LandRequestArgs &);

  virtual std::optional<PullRequestInfo> pullRequestById(const String &);

  virtual std::optional<PullRequestInfo> pullRequestByBranch(const String &);

  virtual bool setPullRequestDraftStatus(const DraftStatusRequest &);

  virtual bool addReviewers(const ReviewersRequest &);

  virtual bool updateTitle(const TitleRequest &);

private:
  const GitServiceFactoryArgs creationArgs_;

  GitlabApiResult gitlabGet(const String &url);
  GitlabApiResult gitlabPost(const String &url, const nlohmann::json &data);
  GitlabApiResult gitlabPut(const String &url, const nlohmann::json &data);

  String getCreateMrUrl();
  String getLandMrUrl(const String &mrNum);
  String getMrByIdUrl(const String &mrNum);
  String getMrByBranchUrl(const String &branch);
  String getPostNotesUrl(const String &mrNum);
  String getUserUrl(const String &user);
  String getCurrentlyAuthenticatedUser();
  String getRebaseMrUrl(const String &mrNum);
  String getMergeTrainByMrIdUrl(const String &mrNum);

  std::optional<LandRequestResult> landPullRequestWithCi(const LandRequestArgs &);

  Vector<String> getHeaders() {
    return {"PRIVATE-TOKEN", creationArgs_.credential, "Content-Type", "application/json"};
  }
};
}  // namespace c8
