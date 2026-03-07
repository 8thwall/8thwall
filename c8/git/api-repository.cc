// Copyright (c) 2019 8th Wall, Inc.
// Original Author: Erik Murphy-Chutorian (mc@8thwall.com)

#include "bzl/inliner/rules2.h"

cc_library {
  visibility = {
    "//visibility:private",
  };
  deps = {
    ":api-common",
    ":g8-api.capnp-cc",
    ":hooks",
    "//c8:c8-log",
    "//c8:c8-log-proto",
    "//c8:scope-exit",
    "//c8:vector",
    "//c8/io:capnp-messages",
    "//c8/stats:scope-timer",
    "//c8/string:format",
    "//c8/string:strcat",
    "@libgit2//:libgit2",
  };
  copts = {"-Iexternal/libgit2/src"};
}
cc_end(0x8e0ec781);

#include <capnp/pretty-print.h>
#include <git2.h>
#include <git2/oid.h>
#include <git2/refs.h>
#include <git2/types.h>

#include <algorithm>
#include <fstream>
#include <iterator>
#include <map>
#include <random>
#include <regex>
#include <set>
#include <sstream>
#include <string>
#include <vector>

#include "c8/c8-log-proto.h"
#include "c8/c8-log.h"
#include "c8/git/api-common.h"
#include "c8/git/hooks.h"
#include "c8/io/capnp-messages.h"
#include "c8/stats/scope-timer.h"
#include "c8/string/format.h"
#include "c8/string/strcat.h"
#include "c8/vector.h"

namespace c8 {

using namespace g8;

ConstRootMessage<G8RepositoryResponse> g8Repository(G8RepositoryRequest::Reader req) {
  ScopeTimer t("api-repository");
  MutableRootMessage<G8RepositoryResponse> response;

  git_clone_options cloneOptions = GIT_CLONE_OPTIONS_INIT;
  git_fetch_options fetchOptions = GIT_FETCH_OPTIONS_INIT;

  auto authReader = req.getAuth();
  fetchOptions.callbacks = gitRemoteCallbacks(authReader);
  fetchOptions.callbacks.payload = reinterpret_cast<void *>(&authReader);

  setRemoteProgressCallbacks(&fetchOptions.callbacks);

  String cred = req.getAuth().hasCred() ? req.getAuth().getCred() : "";
  bool useCreds = !cred.empty();
  auto authStr = format("Authorization: %s", cred.c_str());
  char *authCStr = const_cast<char *>(authStr.c_str());
  if (useCreds) {
    fetchOptions.custom_headers.count = 1;
    fetchOptions.custom_headers.strings = &authCStr;
  }
  cloneOptions.fetch_opts = fetchOptions;

  // TODO(pawel) investigate initial clone when checked out files are git-lfs
  // cloneOptions.checkout_opts.checkout_strategy = GIT_CHECKOUT_NONE;

  // NOTE(nb): Here are some notes from investigating target OID is missing on clone error in .js.
  // This problem is intermittant and hard to repro / debug. Here's something things to try, or
  // info to help in debugging.
  //
  // By default, codecommit connects to remote's HEAD. We have observed this as being pointing to
  // branch OIDs that are synced to master. This might problems with clone. So, explicitly set
  // master as the branch we want to clone.
  //
  // cloneOptions.checkout_branch = "master";
  //
  // To investigate furhter (replace NbTest with your app name):
  // $ git ls-remote --symref https://git-codecommit.us-west-2.amazonaws.com/v1/repos/8w.NbTest
  // $ git remote show https://git-codecommit.us-west-2.amazonaws.com/v1/repos/8w.NbTest

  gitRepositoryInit();
  SCOPE_EXIT([] { git_libgit2_shutdown(); });

  GIT_PTR(repository, repo);

  if (
    req.getAction() == G8RepositoryRequest::Action::CLONE
    || req.getAction() == G8RepositoryRequest::Action::FETCH_NEW_CLIENTS
    || req.getAction() == G8RepositoryRequest::Action::SYNC_MASTER) {
    if (req.getUser().size() == 0) {
      response.builder().getStatus().setCode(1);
      response.builder().getStatus().setMessage(
        "Must set user for a CLONE, FETCH_NEW_CLIENTS, or SYNC_MASTER action");
      return response;
    }
  }

  if (req.getAction() == G8RepositoryRequest::Action::SYNC_MASTER) {
    CHECK_GIT(git_repository_open(&repo, req.getRepo().cStr()));
    CHECK_GIT(g8SyncMaster(repo, req.getAuth(), req.getUser().cStr()));
  }

  if (
    req.getAction() == G8RepositoryRequest::Action::CLONE
    || req.getAction() == G8RepositoryRequest::Action::FETCH_NEW_CLIENTS) {

    if (req.getAction() != G8RepositoryRequest::Action::CLONE && req.hasCloneCheckoutId()) {
      response.builder().getStatus().setCode(1);
      response.builder().getStatus().setMessage("cloneCheckoutId can only be set for CLONE");
      return response;
    }

    String remotePrefix = strCat(ORIGIN, "/", req.getUser().cStr(), "-");
    String remoteMasterName = strCat(ORIGIN, "/master");
    String remoteMainName = strCat(ORIGIN, "/main");

    if (req.getAction() == G8RepositoryRequest::Action::CLONE) {
      const char *refs[] = {
        "main:main",
        "master:master",
        "HEAD",
      };
      char **strings = const_cast<char **>(refs);
      cloneOptions.refspecs = {.strings = strings, .count = sizeof(refs) / sizeof(refs[0])};
      CHECK_GIT(git_clone(&repo, req.getUrl().cStr(), req.getRepo().cStr(), &cloneOptions));
      CHECK_GIT(fetchRemote(repo, req.getAuth(), req.getUser().cStr()));
    } else {
      CHECK_GIT(git_repository_open(&repo, req.getRepo().cStr()));
      // fetch remote branches and refs
      CHECK_GIT(fetchRemote(repo, req.getAuth(), req.getUser().cStr()));
    }

    // Check out all of the user's remote client branches locally.
    {
      GIT_PTR(branch_iterator, iter);
      git_reference *remoteBranch;
      CHECK_GIT(git_branch_iterator_new(&iter, repo, GIT_BRANCH_REMOTE));
      git_branch_t unusedType;
      while (0 == git_branch_next(&remoteBranch, &unusedType, iter)) {
        const char *branchName;
        CHECK_GIT(git_branch_name(&branchName, remoteBranch));

        String branchString = branchName;

        // In case the remote repository HEAD is not set to master/main and the clone doesn't
        // automatically check them out, we'll do that here. G8 assumes that the default branch
        // is main/master.
        bool doMasterCheckout = false;

        if (branchString == remoteMasterName || branchName == remoteMainName) {
          doMasterCheckout = true;
        }

        if (branchString.rfind(remotePrefix, 0) == 0 || doMasterCheckout) {
          // If the remote branch matches the user prefix.
          const char *localBranchName = doMasterCheckout
            ? &branchName[7]  // "ORIGIN/" has 7 characters; for non-user-prefixed branches.
            : &branchName[remotePrefix.size()];
          GIT_PTR(reference, localBranch);

          if (auto res = git_branch_lookup(&localBranch, repo, localBranchName, GIT_BRANCH_LOCAL);
              res == GIT_ENOTFOUND) {
            C8Log("[g8-git] Checking out branch %s", branchName);
            GIT_PTR(commit, branchCommit);
            CHECK_GIT(git_reference_peel(
              reinterpret_cast<git_object **>(&branchCommit), remoteBranch, GIT_OBJECT_COMMIT));
            CHECK_GIT(git_branch_create(&localBranch, repo, localBranchName, branchCommit, 0));
            CHECK_GIT(git_branch_set_upstream(localBranch, branchName));
          } else {
            CHECK_GIT(res);
          }
        }
        git_reference_free(remoteBranch);
      }
    }

    GIT_PTR(remote, remote);
    CHECK_GIT(git_remote_lookup(&remote, repo, ORIGIN));

    // Connect to remote.
    CHECK_GIT(git_remote_connect(
      remote, GIT_DIRECTION_FETCH, &fetchOptions.callbacks, nullptr, &fetchOptions.custom_headers));

    // Disconnect when done.
    SCOPE_EXIT([remote]() { git_remote_disconnect(remote); });

    const git_remote_head **refs;
    size_t refsSize;
    CHECK_GIT(git_remote_ls(&refs, &refsSize, remote));

    for (auto i = 0; i < refsSize; i++) {
      auto ref = refs[i];
      if (String(ref->name) == "HEAD") {
        if (!ref->symref_target) {
          C8Log("WARNING: Remote is broadcasting a HEAD that does not have a symref target.");
        } else {
          String symrefTarget(ref->symref_target);
          if (symrefTarget != "refs/heads/master" && symrefTarget != "refs/heads/main") {
            C8Log("WARNING: Remote HEAD appears to not be pointing to master/main.");
          }
        }
        break;
      }
    }

    // NOTE(pawel) In the cloud editor we need to set author information for the repo since we don't
    // have a global config hanging around. For native g8, we rely on global config author info.
#ifdef JAVASCRIPT
    String email = strCat(req.getUser().cStr(), "@8thwall.user");
    GIT_PTR(config, config);
    git_repository_config(&config, repo);
    if (req.hasAuthorName()) {
      CHECK_GIT(git_config_set_string(config, "user.name", req.getAuthorName().cStr()));
    } else {
      CHECK_GIT(git_config_set_string(config, "user.name", req.getUser().cStr()));
    }
    CHECK_GIT(git_config_set_string(config, "user.email", email.c_str()));
#endif

    String postCheckoutCommit = "0000000000000000000000000000000000000000";

    {
      GIT_PTR(reference, master);
      if (auto res = lookupMainBranch(&master, repo); res == GIT_ENOTFOUND) {
        C8Log(
          "WARNING: main/master branch not found. G8 will not work correctly until this is fixed");
      } else {
        CHECK_GIT(res);
        // Checkout to the master/main branch in case remote HEAD was set to something else.
        // This is so that we start out in a sane place.
        git_checkout_options checkoutOpts = GIT_CHECKOUT_OPTIONS_INIT;
        checkoutOpts.checkout_strategy = GIT_CHECKOUT_FORCE | GIT_CHECKOUT_REMOVE_SPARSE_FILES;

        GIT_PTR(object, treeish);
        CHECK_GIT(git_reference_peel(&treeish, master, GIT_OBJECT_TREE));
        CHECK_GIT(git_checkout_tree(repo, treeish, &checkoutOpts));

        CHECK_GIT(git_repository_set_head(repo, git_reference_name(master)))
      }
    }

    if (req.hasCloneCheckoutId()) {
      git_oid oid;
      GIT_PTR(commit, commit);
      CHECK_GIT(
        git_oid_fromstrn(&oid, req.getCloneCheckoutId().cStr(), req.getCloneCheckoutId().size()));
      CHECK_GIT(git_commit_lookup(&commit, repo, &oid));

      GIT_PTR(reference, oldMaster);
      GIT_PTR(reference, newMaster);

      CHECK_GIT(lookupMainBranch(&oldMaster, repo));
      CHECK_GIT(
        git_reference_set_target(&newMaster, oldMaster, &oid, "Clone checkout to specific commit"));

      git_checkout_options checkoutOpts = GIT_CHECKOUT_OPTIONS_INIT;
      checkoutOpts.checkout_strategy = GIT_CHECKOUT_FORCE | GIT_CHECKOUT_REMOVE_SPARSE_FILES;

      GIT_PTR(object, treeish);
      CHECK_GIT(git_reference_peel(&treeish, newMaster, GIT_OBJECT_TREE));
      CHECK_GIT(git_checkout_tree(repo, treeish, &checkoutOpts));

      CHECK_GIT(git_repository_set_head(repo, git_reference_name(newMaster)))

      postCheckoutCommit = req.getCloneCheckoutId().cStr();
    }

    // post-checkout hook; since we arrive on master initially
    {
      auto postCheckoutRes = postCheckoutHook(
        repo,
        postCheckoutCommit.c_str(),
        "/refs/heads/master",  // new HEAD
        CheckoutType::BRANCH);
      if (postCheckoutRes && *postCheckoutRes != 0) {
        // TODO(pawel) notify hook returned non-zero value?
      }
    }
  }

  if (req.getAction() == G8RepositoryRequest::Action::GET_COMMIT_LOG) {
    CHECK_GIT(git_repository_open(&repo, req.getRepo().cStr()));

    GIT_PTR(revwalk, walk);
    CHECK_GIT(git_revwalk_new(&walk, repo));

    git_oid oid;
    if (req.getLogStartId().size() > 0) {
      CHECK_GIT(git_oid_fromstr(&oid, req.getLogStartId().cStr()));
    } else {
      GIT_PTR(reference, master);
      CHECK_GIT(lookupMainBranch(&master, repo));
      oid = *git_reference_target(master);
    }

    CHECK_GIT(git_revwalk_push(walk, &oid));

    // Sane default log depth
    const int entries = req.getLogDepth() ? req.getLogDepth() : 25;
    std::vector<git_oid> oids;
    oids.reserve(entries);
    for (int i = 0; i < entries; i++) {
      auto res = git_revwalk_next(&oid, walk);
      if (res == GIT_ITEROVER) {
        break;
      }
      CHECK_GIT(res);
      oids.push_back(oid);
    }

    response.builder().initCommitLog(oids.size());
    for (int i = 0; i < oids.size(); i++) {
      GIT_PTR(commit, commit);
      CHECK_GIT(git_commit_lookup(&commit, repo, &oids[i]));
      auto signature = git_commit_author(commit);

      auto messageOrEmpty = [](const char *msg) { return msg ? msg : ""; };

      auto commitResponse = response.builder().getCommitLog()[i];
      commitResponse.setId(git_oid_tostr_s(&oids[i]));
      commitResponse.setParentId(git_oid_tostr_s(git_commit_parent_id(commit, 0)));
      commitResponse.setSummary(messageOrEmpty(git_commit_summary(commit)));
      commitResponse.setDescription(messageOrEmpty(git_commit_message(commit)));
      commitResponse.setBody(messageOrEmpty(git_commit_body(commit)));
      commitResponse.getSignature().setName(signature->name);
      commitResponse.getSignature().setEmail(signature->email);
      commitResponse.getSignature().setWhen(signature->when.time);
    }
  }

  if (req.getAction() == G8RepositoryRequest::Action::INITIAL_COMMIT) {
    if (req.getMessage().size() == 0) {
      response.builder().getStatus().setCode(1);
      response.builder().getStatus().setMessage("must specify a message for initial commit");
      return response;
    }

    CHECK_GIT(git_repository_open(&repo, req.getRepo().cStr()));

    GIT_PTR(reference, master);
    if (auto res = lookupMainBranch(&master, repo); res == GIT_ENOTFOUND) {

      const char *glob = "*";
      git_strarray strs = {
        .strings = (char **)&glob,
        .count = 1,
      };

      GIT_PTR(index, index);
      git_oid treeOid;
      GIT_PTR(tree, tree);
      git_oid commitOid;

      CHECK_GIT(git_repository_index(&index, repo));
      CHECK_GIT(git_index_read(index, true));
      CHECK_GIT(git_index_add_all(index, &strs, 0, nullptr, nullptr));
      CHECK_GIT(git_index_write_tree(&treeOid, index));
      CHECK_GIT(git_index_write(index));
      CHECK_GIT(git_tree_lookup(&tree, repo, &treeOid));

      GIT_PTR(signature, signature);
      CHECK_GIT(git_signature_default(&signature, repo));

      CHECK_GIT(git_commit_create(
        &commitOid,
        repo,
        "refs/heads/master",
        signature,
        signature,
        nullptr,
        req.getMessage().cStr(),
        tree,
        0,
        nullptr));

      // post-commit
      if (auto postCommitRes = postCommitHook(repo); postCommitRes && *postCommitRes != 0) {
        // TODO(pawel) report non-zero exit status?
      }

      git_push_options pushOpts = GIT_PUSH_OPTIONS_INIT;
      auto authReader = req.getAuth();
      pushOpts.callbacks = gitRemoteCallbacks(authReader);
      pushOpts.callbacks.payload = reinterpret_cast<void *>(&authReader);

      String cred = authReader.hasCred() ? authReader.getCred() : "";
      bool useCreds = !cred.empty();
      auto authStr = format("Authorization: %s", cred.c_str());
      char *authCStr = const_cast<char *>(authStr.c_str());
      if (useCreds) {
        pushOpts.custom_headers.count = 1;
        pushOpts.custom_headers.strings = &authCStr;
      }

      char *refs[] = {
        const_cast<char *>("refs/heads/master"),
      };
      git_strarray specs = {refs, 1};

      GIT_PTR(remote, remote);
      CHECK_GIT(git_remote_lookup(&remote, repo, ORIGIN));

      // pre push
      {
        auto prePushRes = prePushHook(
          repo,
          git_remote_name(remote),
          git_remote_url(remote),
          "/refs/heads/master",
          git_oid_tostr_s(&commitOid),
          "/refs/heads/master",
          "0000000000000000000000000000000000000000"  // foreign ref does not exist
        );
        if (prePushRes && *prePushRes != 0) {
          // TODO(pawel) what does it mean for initial commit push to fail??
          // TODO(pawel) how to handle?
        }
      }

      int remoteResult = git_remote_push(remote, &specs, &pushOpts);
      if (0 != remoteResult) {
        const git_error *err = git_error_last();
        int code = err->klass;
        String msg = err->message;
        git_error_set_str(code, msg.c_str());
        CHECK_GIT(remoteResult);
      }

    } else {
      CHECK_GIT(res);
      response.builder().getStatus().setCode(1);
      response.builder().getStatus().setMessage(
        "refusing to create initial commit: master already exists");
      return response;
    }
  }

  if (req.getAction() == G8RepositoryRequest::Action::GET_INFO) {
    // open given repo
    CHECK_GIT(git_repository_open(&repo, req.getRepo().cStr()));

    GIT_PTR(remote, remote);
    CHECK_GIT(git_remote_lookup(&remote, repo, ORIGIN));
    setRemoteInfo(response.builder().getRemote(), remote);

    GIT_PTR(reference, master);
    CHECK_GIT(lookupMainBranch(&master, repo));

    response.builder().setMainBranchName(git_reference_shorthand(master));
  }

  if (req.getAction() == G8RepositoryRequest::Action::KEEP_HOUSE) {
    if (!req.hasUser()) {
      response.builder().getStatus().setCode(1);
      response.builder().getStatus().setMessage("User needs to be set for house keeping.");
      return response;
    }
    CHECK_GIT(git_repository_open(&repo, req.getRepo().cStr()));

    String configFilePath = strCat(git_repository_workdir(repo), "/.git/config");
    // There are no libgit2 functions that allow us to remove empty sections in the config file...
    // We tag section headers and empty section headers are obliterated.
    {
      std::regex sectionHeaderRegex(R"(^\[branch [^\]]*\]$)");
      Vector<uint8_t> lineIsSectionHeader;
      Vector<String> lines;
      {
        std::ifstream configFileInStream(configFilePath);
        String line;
        while (getline(configFileInStream, line)) {
          auto res = std::regex_match(line, sectionHeaderRegex);
          lineIsSectionHeader.push_back(res ? 1 : 0);
          lines.push_back(line);
        }
      }

      String output;
      const auto numLines = lines.size();
      for (int idx = 0; idx < numLines - 1; idx++) {
        if (lineIsSectionHeader[idx] == 1 && lineIsSectionHeader[idx + 1] == 1) {
          continue;
        }
        output += lines[idx] + "\n";
      }
      if (numLines > 1 && lineIsSectionHeader[numLines - 1] != 1) {
        output += lines[lines.size() - 1] + "\n";
      }

      std::ofstream configFileOutStream(configFilePath);
      configFileOutStream << output;
    }

    GIT_PTR(config, config);
    CHECK_GIT(git_config_open_ondisk(&config, configFilePath.c_str()));

    {
      const auto [lsClientsRes, clientBranches, _] = g8GitLsClients(repo);
      CHECK_GIT(lsClientsRes);

      Vector<String> branchNamesToCheck;

      // Get list of all g8 clients and changesets.
      // NOTE(pawel) Any branch who's parent commit is on the master line is considered a client.
      // This means that local branches that fit the bill for client but have no remote tracking
      // branches will get a g8-style remote branch. This is because we have no other way of knowing
      // what branch was created as a "client" by g8.
      for (const auto &clientName : clientBranches) {
        branchNamesToCheck.push_back(clientName);
        const auto [lsGitRes, csList] = g8GitLsChangesets(repo, clientName.c_str());
        CHECK_GIT(lsGitRes);
        for (const auto &cs : csList.changesets) {
          // cs.branchName is actuall the full ref e.g. refs/heads/buggs-CS671612
          const auto &refName = cs.branchName;
          branchNamesToCheck.push_back(refName.substr(refName.rfind("/") + 1));
        }
      }

      // Check to make sure config file has upstream entries. Create missing upstream references.
      for (const auto &branchName : branchNamesToCheck) {
        GIT_PTR(reference, branchRef);
        GIT_PTR(reference, clientUpstream);
        git_buf unusedBuf{nullptr, 0, 0};

        CHECK_GIT(git_branch_lookup(&branchRef, repo, branchName.c_str(), GIT_BRANCH_LOCAL));

        if (auto gitRes =
              git_branch_upstream_remote(&unusedBuf, repo, git_reference_name(branchRef));
            gitRes == GIT_ENOTFOUND) {
          git_config_set_string(config, strCat("branch.", branchName, ".remote").c_str(), ORIGIN);
        } else {
          CHECK_GIT(gitRes);
        }

        GIT_PTR(config, snapshotConfig);
        CHECK_GIT(git_config_snapshot(&snapshotConfig, config));
        const char *merge{0};
        if (auto gitRes = git_config_get_string(
              &merge, snapshotConfig, strCat("branch.", branchName, ".merge").c_str());
            gitRes == GIT_ENOTFOUND) {
          git_config_set_string(
            config,
            strCat("branch.", branchName, ".merge").c_str(),
            strCat("refs/heads/", req.getUser().cStr(), "-", branchName).c_str());
        } else {
          CHECK_GIT(gitRes);
        }

        GIT_PTR(commit, commit);
        CHECK_GIT(git_reference_peel(
          reinterpret_cast<git_object **>(&commit), branchRef, GIT_OBJECT_COMMIT));

        // This create the reference if it does not already exist, otherwise returns GIT_EEXISTS.
        const auto remoteName =
          strCat("refs/remotes/", ORIGIN, "/", req.getUser().cStr(), "-", branchName);
        GIT_PTR(reference, remoteRef);
        if (auto gitRes = git_reference_create(
              &remoteRef,
              repo,
              remoteName.c_str(),
              git_commit_id(commit),
              false,  // force
              "Recreate missing remote tracking branch.");
            gitRes != GIT_EEXISTS) {
          CHECK_GIT(gitRes);
        }
      }
    }

    // TODO(pawel) Delete subsections for branches which no longer exist.
    // Iterate over all local branches, go through branch config headers and remove
    // sections that were not found in the branch iteration.
  }

  response.builder().getStatus().setCode(0);
  response.builder().setRepo(git_repository_workdir(repo));

  return response;
}

}  // namespace c8
