// Copyright (c) 2022 8th Wall, LLC.
// Original Author: Pawel Czarnecki (pawel@nianticlabs.com)

#pragma once

#include <optional>

#include "c8/string.h"
#include "c8/vector.h"

namespace c8 {

enum struct GitServiceType {
  UNSPECIFIED,
  GITHUB,
  GITLAB,
};

/**
 * @brief Arguments for creating the git service endpoint.
 * @param serviceType The API interface of the remote service.
 * @param remoteName The name of the remote repository to connect to.
 * @param endpoint The server address to use. Services like github and gitlab have default
 * remote addresses (github.com and gitlab.com) but they can also be self hosted. Set this
 * field to override the default endpoint (such as for Niantic gitlab).
 */
struct GitServiceFactoryArgs {
  GitServiceType serviceType{GitServiceType::UNSPECIFIED};
  String repoName;                // Name of the remote repository.
  String endpoint;                // Specify the endpoint server address.
  String mainBranchName{"main"};  // master or main.
  String credential;              // This is what is included in the Authorization header.
  bool verbose{false};
};

struct CreatePullRequestArgs {
  String headRef;
  String title;
  String body;
  bool draft{true};
};

struct CreatePullRequestResult {
  String pullRequestId;
};

/**
 * @brief Land request is a squash and merge a pull/merge request.
 * @param expectedTipSha The commit id of the tip of the changeset branch.
 * This ensures that the remote changeset matches the local one (i.e. is up to date).
 * It also is a sanity check to ensure that the pullRequestId corresponds to this changeset.
 * @param pullRequestId The PR/MR number on the remote service.
 */
struct LandRequestArgs {
  String pullRequestId;
  String expectedTipSha;
  String commitTitle;
  String commitMessage;
  bool withCi{false};
};

/**
 * @brief Record of successful land.
 * @param landedMasterSha The resultant master sha after land. This is the commit to which
 * we want to sync to locally.
 */
struct LandRequestResult {
  String landedMasterSha;
};

/**
 * @brief Result of querying the remote PR/MR by branch name or pullRequestId.
 */
struct PullRequestInfo {
  String branchName;
  String pullRequestId;
  String url;
  bool isDraft;
  bool isMerged{false};
  // The optionals below are only set if this bool is true.
  bool isValidG8RemoteChangesetBranch{false};
  std::optional<String> userName;
  std::optional<String> clientName;
  std::optional<String> changesetId;
};

/**
 * @brief Used to set draft status.
 * @param pullRequestId The PR/MR to modify.
 * @param shouldBeDraft Whether it should be a draft or not.
 */
struct DraftStatusRequest {
  String pullRequestId;
  bool shouldBeDraft;
};

/**
 * @brief Used to add reviewers on PR/MR.
 * Note that for some services (e.g. GitHub) adding the author as a reviewer
 * will fail with an error.
 * @param reviewers A list of user names of the service to be added as reviewers.
 */
struct ReviewersRequest {
  String pullRequestId;
  Vector<String> reviewers;
};

/**
 * @brief Used to update title/message on PR/MR.
 * @param message A string to be updated as the title message.
 */
struct TitleRequest {
  String pullRequestId;
  String message;
};

class GitServiceInterface {
public:
  explicit GitServiceInterface() = default;
  virtual ~GitServiceInterface() = default;

  GitServiceInterface(const GitServiceInterface &) = delete;
  GitServiceInterface &operator=(GitServiceInterface &) = delete;

  // Returns the PR/MR number if create was successful.
  virtual std::optional<CreatePullRequestResult> createPullRequest(
    const CreatePullRequestArgs &) = 0;

  // Returns the new master sha if land successful.
  virtual std::optional<LandRequestResult> landPullRequest(const LandRequestArgs &) = 0;

  // Get information about a particular pull request.
  virtual std::optional<PullRequestInfo> pullRequestById(const String &) = 0;

  // Find the pull request id for a given branch.
  virtual std::optional<PullRequestInfo> pullRequestByBranch(const String &) = 0;

  // Set draft status for PR. Returns true if successful.
  virtual bool setPullRequestDraftStatus(const DraftStatusRequest &) = 0;

  // Adds reviewers and return if successful.
  virtual bool addReviewers(const ReviewersRequest &) = 0;

  // Update title and return if successful.
  virtual bool updateTitle(const TitleRequest &) = 0;
};
}  // namespace c8
