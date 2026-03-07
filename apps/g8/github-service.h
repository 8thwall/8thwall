// Copyright (c) 2022 8th Wall, LLC.
// Original Author: Pawel Czarnecki (pawel@nianticlabs.com)

#pragma once

#include "apps/g8/git-service-interface.h"
#include "c8/map.h"
#include "nlohmann/json.hpp"

namespace c8 {

class GithubService : public GitServiceInterface {
public:
  explicit GithubService(GitServiceFactoryArgs args) : creationArgs_(args) {}

  virtual std::optional<CreatePullRequestResult> createPullRequest(const CreatePullRequestArgs &);

  virtual std::optional<LandRequestResult> landPullRequest(const LandRequestArgs &);

  virtual std::optional<PullRequestInfo> pullRequestById(const String &);

  virtual std::optional<PullRequestInfo> pullRequestByBranch(const String &);

  virtual bool setPullRequestDraftStatus(const DraftStatusRequest &);

  virtual bool addReviewers(const ReviewersRequest &);

  virtual bool updateTitle(const TitleRequest &);

private:
  // Because marking as draft need graphql api and we need the graphql node id,
  // we'll keep an internal mapping for pr number -> pr node id.
  HashMap<String, String> prNumberToGraphqlId_;

  const GitServiceFactoryArgs creationArgs_;

  template <typename... Args>
  std::optional<nlohmann::json> curlGithub(Args &&...args);
};
}  // namespace c8
