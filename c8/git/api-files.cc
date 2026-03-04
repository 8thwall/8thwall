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
cc_end(0x5b7ef7ea);

#include <capnp/pretty-print.h>
#include <git2.h>
#include <git2/oid.h>
#include <git2/refs.h>
#include <git2/types.h>
#include <sys/stat.h>

#include <algorithm>
#include <filesystem>
#include <map>
#include <random>
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
namespace fs = std::filesystem;

namespace {
static mode_t getUmaskDefault() {
  static mode_t umaskDefault = 0;
  static bool initialized = false;

  if (!initialized) {
    // The umask function has an unusual API. It returns the current umask and sets the umask in the
    // same call. To simply read the current umask, we set it to zero and then restore it.
    umaskDefault = umask(0);  // Get umask and set to zero.
    umask(umaskDefault);      // Restore original umask.
    initialized = true;
  }

  return umaskDefault;
}

mode_t getSymlinkMode() {
#if defined(__FreeBSD__) || defined(__NetBSD__) || defined(__OpenBSD__) || defined(__APPLE__)
  // On BSD-unix, symlinks have permissions and default to 777 - umask.
  return (S_IFLNK | S_IRWXU | S_IRWXG | S_IRWXO) - getUmaskDefault();
#else
  // On other unix systems, symlinks don't have permisisons and default to 777.
  return S_IFLNK | S_IRWXU | S_IRWXG | S_IRWXO;
#endif
}

// Method to add a workdir entry to a vector of G8FileInfo.
G8FileInfo::Builder addWorkdirEntryToOutput(
  const struct stat &fileStat,
  const fs::path &fullPath,
  const char *path,
  const char *label,
  Vector<std::unique_ptr<MutableRootMessage<G8FileInfo>>> &output) {
  auto &msg = output.emplace_back(new MutableRootMessage<G8FileInfo>);
  auto fileInfo = msg->builder();
  fileInfo.setPath(path);
  if (label) {
    fileInfo.setLabel(label);
  }
  fileInfo.setMode(fileStat.st_mode);
  fileInfo.setUid(fileStat.st_uid);
  fileInfo.setGid(fileStat.st_gid);
  fileInfo.setByteSize(fileStat.st_size);
  fileInfo.setMtime(fileStat.st_mtime);
  fileInfo.setCtime(fileStat.st_ctime);
  fileInfo.setInode(fileStat.st_ino);
  fileInfo.setLinkCount(fileStat.st_nlink);
  fileInfo.setSparseNoCheckout(false);
  if (S_ISLNK(fileStat.st_mode)) {
    // If it's a symlink, let's read the link target.
    fileInfo.setSymlinkTarget(fs::read_symlink(fullPath).c_str());
  }
  return fileInfo;
};

// Method to add an index entry to a vector of G8FileInfo.
G8FileInfo::Builder addIndexEntryToOutput(
  git_repository *repo,
  const char *path,
  const char *label,
  const git_index_entry *entry,
  Vector<std::unique_ptr<MutableRootMessage<G8FileInfo>>> &output) {
  auto &msg = output.emplace_back(new MutableRootMessage<G8FileInfo>);
  auto fileInfo = msg->builder();
  fileInfo.setPath(path);
  if (label) {
    fileInfo.setLabel(label);
  }
  fileInfo.setUid(entry->uid);
  fileInfo.setGid(entry->gid);
  fileInfo.setByteSize(entry->file_size);
  fileInfo.setMtime(entry->mtime.seconds);
  fileInfo.setCtime(entry->ctime.seconds);
  fileInfo.setInode(entry->ino);
  fileInfo.setLinkCount(1);
  fileInfo.setSparseNoCheckout(true);
  if (S_ISLNK(entry->mode)) {
    // Set the symlink mode from the system default, as git doesn't store permissions for
    // symlinks.
    fileInfo.setMode(getSymlinkMode());
  } else {
    fileInfo.setMode(entry->mode);
  }
  // We can set more fields if we happen to have the blob on disk.
  GIT_PTR(blob, blob);
  if (0 == git_blob_lookup(&blob, repo, &entry->id)) {
    // Use the blob size as the byte size, as the index byte size for sparse files
    // doesn't appear to be accurate, at least not for our current libgit2
    // implementation.
    fileInfo.setByteSize(git_blob_rawsize(blob));

    // Set the symlink target.
    if (S_ISLNK(entry->mode)) {
      fileInfo.setSymlinkTarget(capnp::Text::Reader(
        reinterpret_cast<const char *>(git_blob_rawcontent(blob)), git_blob_rawsize(blob)));
    }
  }
  return fileInfo;
};

// Method to add an inferred directory index entry to a vector of G8FileInfo.
G8FileInfo::Builder addInferredDirectoryIndexEntryToOutput(
  const char *path,
  const char *label,
  int linkCount,
  const git_index_entry *entry,
  Vector<std::unique_ptr<MutableRootMessage<G8FileInfo>>> &output) {
  auto &msg = output.emplace_back(new MutableRootMessage<G8FileInfo>);
  auto info = msg->builder();
  info.setPath(path);
  if (label) {
    info.setLabel(label);
  }
  info.setMode(0040755);
  info.setSparseNoCheckout(true);
  info.setLinkCount(linkCount);
  info.setByteSize(linkCount * 32);
  // These coorespond to a file we enountered, but are good proxies for the
  // directory.
  info.setUid(entry->uid);
  info.setGid(entry->gid);
  info.setMtime(entry->mtime.seconds);
  info.setCtime(entry->ctime.seconds);
  return info;
};

}  // namespace

ConstRootMessage<G8MergeFileResponse> g8MergeFile(G8MergeFileRequest::Reader req) {
  ScopeTimer t("api-merge-files");
  MutableRootMessage<G8MergeFileResponse> response;

  gitRepositoryInit();
  SCOPE_EXIT([] { git_libgit2_shutdown(); });

  // Open the repo.
  GIT_PTR(repository, repo);
  CHECK_GIT(git_repository_open(&repo, req.getRepo().cStr()));

  // Set the repo root in the response.
  response.builder().getStatus().setCode(0);
  response.builder().setRepo(git_repository_workdir(repo));

  GIT_PTR(odb, odb);
  CHECK_GIT(git_repository_odb(&odb, repo));

  // Lambda to get the merge_file_input from commit oid and path.
  auto getMergeInput = [odb](
                         capnp::Text::Reader path,
                         capnp::Text::Reader fileOid,
                         G8MergeFileInfo::Builder info,
                         MergeFileState *st) -> int {
    GIT_STATUS_RETURN(git_oid_fromstrp(&st->fileOid, fileOid.cStr()));
    GIT_STATUS_RETURN(git_odb_read(&st->odbObject, odb, &st->fileOid));
    st->mergeFileInput.ptr = reinterpret_cast<const char *>(git_odb_object_data(st->odbObject));
    st->mergeFileInput.size = git_odb_object_size(st->odbObject);
    st->mergeFileInput.path = path.cStr();
    info.setPath(st->mergeFileInput.path);
    return 0;
  };

  MergeFileState original, theirs, yours;
  auto mergeBuilder = response.builder().getMerge();
  if (req.getFileId().hasOriginal()) {
    CHECK_GIT(getMergeInput(
      req.getPath(), req.getFileId().getOriginal(), mergeBuilder.getOriginal(), &original));
  }
  if (req.getFileId().hasTheirs()) {
    CHECK_GIT(
      getMergeInput(req.getPath(), req.getFileId().getTheirs(), mergeBuilder.getTheirs(), &theirs));
  }
  if (req.getFileId().hasYours()) {
    CHECK_GIT(
      getMergeInput(req.getPath(), req.getFileId().getYours(), mergeBuilder.getYours(), &yours));
  }

  constexpr int MARKER_SIZE = 7;

  git_merge_file_result mergeResult;
  git_merge_file_options mergeFileOpts = GIT_MERGE_FILE_OPTIONS_INIT;
  mergeFileOpts.marker_size = MARKER_SIZE;
  mergeFileOpts.ancestor_label = "ORIGINAL";
  mergeFileOpts.their_label = "THEIRS";
  mergeFileOpts.our_label = "YOURS";
  mergeFileOpts.flags = static_cast<git_merge_file_flag_t>(GIT_MERGE_FILE_STYLE_DIFF3);
  CHECK_GIT(git_merge_file(
    &mergeResult,
    &original.mergeFileInput,
    &yours.mergeFileInput,
    &theirs.mergeFileInput,
    &mergeFileOpts));

  String data(mergeResult.ptr, mergeResult.len);

  // Get a pointer to the start of each line, plus one sentinel at the end.
  std::vector<const char *> lines;
  lines.push_back(mergeResult.ptr);

  // Get pointers into all of the lines in the input data.
  for (int i = 0; i < mergeResult.len; ++i) {
    if (mergeResult.ptr[i] == '\n') {
      lines.push_back(mergeResult.ptr + i + 1);
    }
  }

  // Add a trailing sentinel pointer if there isn't a trailing newline.
  if (mergeResult.ptr[mergeResult.len - 1] != '\n') {
    lines.push_back(mergeResult.ptr + mergeResult.len);
  }

  // NOTE(mc): A Better way to do this would be to use xdiff to return all of the 3-way
  // merge hunks and send that back directly. However, compiling xdiff and libgit2
  // directly won't work, since libgit2 has xdiff symbols from a version that predates
  // 3-way merge hunks. I assume it could be done by dynamically linking the xdiff library
  // or putting it into a different JS module, or forking and namespacing the symbols.
  // None of these are very attractive options, currently, so instead we are just
  // iterating over the result of a 3-way merge and inspecting the merge markers.

  // Count the hunks.
  int hunks = 0;
  for (int i = 0; i < lines.size() - 1; ++i) {
    const char *start = lines[i];
    int len = lines[i + 1] - lines[i];
    if (0 == strncmp("<<<<<<< YOURS\n", start, len)) {
      ++hunks;
    }
  }

  // Initialize Hunks
  mergeBuilder.initHunk(hunks);

  MergeSegmentType type = MergeSegmentType::COMMON;

  G8MergeHunk::Builder hunk(nullptr);
  int hunkIdx = -1;
  for (int i = 0; i < lines.size() - 1; ++i) {
    const char *start = lines[i];
    auto len = lines[i + 1] - lines[i];

    if (0 == strncmp("<<<<<<< YOURS\n", start, len)) {
      hunk = mergeBuilder.getHunk()[++hunkIdx];
      hunk.getYours().setStart(i + 1);
      hunk.getYours().setSize(0);
      type = MergeSegmentType::YOURS;
    } else if (hunkIdx < 0) {
      // We haven't found the first hunk.
      continue;
    } else if (0 == strncmp("||||||| ORIGINAL\n", start, len)) {
      hunk.getOriginal().setStart(i + 1);
      hunk.getOriginal().setSize(0);
      type = MergeSegmentType::ORIGINAL;
    } else if (0 == strncmp("=======\n", start, len)) {
      hunk.getTheirs().setStart(i + 1);
      hunk.getTheirs().setSize(0);
      type = MergeSegmentType::THEIRS;
    } else if (0 == strncmp(">>>>>>> THEIRS\n", start, len)) {
      type = MergeSegmentType::COMMON;
    } else {
      switch (type) {
        case MergeSegmentType::ORIGINAL:
          hunk.getOriginal().setSize(hunk.getOriginal().getSize() + 1);
          break;
        case MergeSegmentType::THEIRS:
          hunk.getTheirs().setSize(hunk.getTheirs().getSize() + 1);
          break;
        case MergeSegmentType::YOURS:
          hunk.getYours().setSize(hunk.getYours().getSize() + 1);
          break;
        case MergeSegmentType::COMMON:
          // Nothing to do.
          break;
      }
    }
  }

  std::stringstream output;

  int line = 0;
  // String line;
  char shortOid[8] = {'\0'};
  for (auto hunk : mergeBuilder.getHunk()) {
    // Write all the common lines until the start of the conflict.
    while (line < hunk.getYours().getStart() - 1 && line < lines.size() - 1) {
      const char *start = lines[line];
      auto len = lines[line + 1] - lines[line];
      output.write(start, len);
      ++line;
    }
    git_oid oid;
    // Write out the ORIGINAL lines.
    CHECK_GIT(git_oid_fromstrp(&oid, req.getMergeId().getOriginal().cStr()));
    git_oid_tostr(shortOid, sizeof(shortOid), &oid);
    output << ">>>> ORIGINAL " << mergeBuilder.getOriginal().getPath().cStr() << "#" << shortOid
           << std::endl;
    for (int i = hunk.getOriginal().getStart();
         i < hunk.getOriginal().getStart() + hunk.getOriginal().getSize();
         ++i) {
      const char *start = lines[i];
      auto len = lines[i + 1] - lines[i];
      output.write(start, len);
    }

    // Write out the THEIRS lines.
    CHECK_GIT(git_oid_fromstrp(&oid, req.getMergeId().getTheirs().cStr()));
    git_oid_tostr(shortOid, sizeof(shortOid), &oid);
    output << "==== THEIRS " << mergeBuilder.getTheirs().getPath().cStr() << "#" << shortOid
           << std::endl;
    for (int i = hunk.getTheirs().getStart();
         i < hunk.getTheirs().getStart() + hunk.getTheirs().getSize();
         ++i) {
      const char *start = lines[i];
      auto len = lines[i + 1] - lines[i];
      output.write(start, len);
    }

    // Write out the YOURS lines.
    output << "==== YOURS " << mergeBuilder.getYours().getPath().cStr() << std::endl;
    for (int i = hunk.getYours().getStart();
         i < hunk.getYours().getStart() + hunk.getYours().getSize();
         ++i) {
      const char *start = lines[i];
      auto len = lines[i + 1] - lines[i];
      output.write(start, len);
    }

    // Swap the starting positions for the diff style we are using (ORIGINAL, THEIRS,
    // YOURS).
    hunk.getOriginal().setStart(hunk.getYours().getStart());
    hunk.getTheirs().setStart(hunk.getOriginal().getStart() + hunk.getOriginal().getSize() + 1);
    hunk.getYours().setStart(hunk.getTheirs().getStart() + hunk.getTheirs().getSize() + 1);

    // Write out the hunk terminator.
    output << "<<<<\n";
    line = hunk.getYours().getStart() + hunk.getYours().getSize() + 1;
  }
  // Write out whatever remains in the file.
  while (line < lines.size() - 1) {
    const char *start = lines[line];
    auto len = lines[line + 1] - lines[line];
    output.write(start, len);
    ++line;
  }

  // Set the content in the response.
  mergeBuilder.setContent(output.str());

  // Free the merge file. We don't use GIT_PTR, since the allocator doesn't take a pointer.
  git_merge_file_result_free(&mergeResult);

  return response;
}

ConstRootMessage<G8FileResponse> g8FileRequest(G8FileRequest::Reader req) {
  ScopeTimer t("api-file-request");
  MutableRootMessage<G8FileRequest> rewrittenRequestMessage(req);
  req = rewrittenRequestMessage.reader();
  MutableRootMessage<G8FileResponse> response;

  // Initialize libgit2
  gitRepositoryInit();
  SCOPE_EXIT([] { git_libgit2_shutdown(); });

  // Open repository
  GIT_PTR(repository, repo);
  CHECK_GIT(git_repository_open(&repo, req.getRepo().cStr()));

  // Set response repo root
  response.builder().getStatus().setCode(0);
  response.builder().setRepo(git_repository_workdir(repo));

  GIT_PTR(reference, master);
  CHECK_GIT(lookupMainBranch(&master, repo));

  MutableRootMessage<G8FileInfoList> modifiedClientFiles;
  std::map<String, G8FileInfo::Builder> processed;
  std::map<String, G8FileInfo::Builder> toProcess;

  switch (req.getAction()) {
    case G8FileRequest::Action::REVERT: {
      // Grab reference to client branch
      GIT_PTR(reference, client);
      if (req.getClient().size() == 0) {
        response.builder().getStatus().setCode(1);
        response.builder().getStatus().setMessage("No active client specified");
        return response;
      } else if (req.getClient() == "HEAD" || req.getClient() == "head") {
        const char *clientFullName{nullptr};
        GIT_PTR(reference, head);
        CHECK_GIT(git_reference_lookup(&head, repo, "HEAD"));

        clientFullName = git_reference_symbolic_target(head);
        if (!clientFullName) {
          response.builder().getStatus().setCode(1);
          response.builder().getStatus().setMessage("No active client in repository");
          return response;
        }

        CHECK_GIT(git_reference_lookup(&client, repo, clientFullName));

        rewrittenRequestMessage.builder().setClient(git_reference_shorthand(client));
        req = rewrittenRequestMessage.reader();
        response.builder().setClient(git_reference_shorthand(client));
      } else {
        CHECK_GIT(git_branch_lookup(&client, repo, req.getClient().cStr(), GIT_BRANCH_LOCAL));
      }

      GIT_PTR(commit, revertCommit);
      if (req.hasRevertPoint()) {
        // Look up by commit hash.
        GIT_PTR(object, object);
        CHECK_GIT(git_revparse_single(&object, repo, req.getRevertPoint().cStr()));
        CHECK_GIT(git_commit_lookup(&revertCommit, repo, git_object_id(object)));

      } else {
        // Default is to revert to change point.
        CHECK_GIT(gitFindMergeBase(&revertCommit, repo, master, client));
      }

      GIT_PTR(tree, clientForkTree);
      CHECK_GIT(resolveTreeForChangePoint(repo, &clientForkTree, "FORK"));

      // Get modified files in client
      GIT_PTR(index, workIndex);
      CHECK_GIT(makeWorkIndex(repo, &workIndex, clientForkTree));
      GIT_PTR(tree, revertTree);
      CHECK_GIT(git_commit_tree(&revertTree, revertCommit));

      // NOTE(pawel) Sides of diff don't matter as we only care about modified paths.
      CHECK_GIT(gitDiffTreeToIndex(
        repo, revertTree, workIndex, false, nullptr, modifiedClientFiles.builder()));

      if (req.getPaths().size() == 0) {
        // Add all files to the request
        rewrittenRequestMessage.builder().initPaths(modifiedClientFiles.reader().getInfo().size());
        for (int i = 0; i < modifiedClientFiles.reader().getInfo().size(); i++) {
          rewrittenRequestMessage.builder().getPaths().set(
            i, modifiedClientFiles.reader().getInfo()[i].getPath());
        }
        req = rewrittenRequestMessage.reader();
      }

      std::set<String> filesToRevert;
      for (const auto path : req.getPaths()) {
        filesToRevert.insert(path.cStr());
      }

      if (req.hasPathsToPreserve()) {
        for (const auto preservedPath : req.getPathsToPreserve()) {
          filesToRevert.erase(preservedPath);
        }
      }

      for (auto file : modifiedClientFiles.builder().getInfo()) {
        if (
          std::find(begin(filesToRevert), end(filesToRevert), String(file.getPath().cStr()))
          != end(filesToRevert)) {
          toProcess.insert(std::make_pair(file.getPath().cStr(), file));
        }
      }

      auto workdir = git_repository_workdir(repo);

      for (auto [path, file] : toProcess) {
        auto fullPath = path[0] == '/' ? strCat(workdir, "/", path) : strCat(workdir, path);

        GIT_PTR(tree_entry, entry);
        if (auto res = git_tree_entry_bypath(&entry, revertTree, path.c_str());
            res != GIT_ENOTFOUND) {
          CHECK_GIT(res);
        } else {
          file.setStatus(G8FileInfo::Status::DELETED);
          file.setDirty(false);
          processed.insert(std::make_pair(path, file));
          if (!req.getDryRun()) {
            if (remove(fullPath.c_str())) {
              response.builder().getStatus().setCode(1);
              response.builder().getStatus().setMessage(
                strCat("Failed to delete file ", fullPath.c_str()));
              break;
            }
          }
          continue;  // Done with this file.
        }

        GIT_PTR(object, object);
        CHECK_GIT(git_tree_entry_to_object(&object, repo, entry));

        if (auto type = git_object_type(object); type == GIT_OBJECT_BLOB) {
          auto blob = reinterpret_cast<git_blob *>(object);

          if (!req.getDryRun()) {
            auto f = fopen(fullPath.c_str(), "w+");
            if (!f) {
              // Folder may not exist so create it and open the file again.
              fs::path filename(fullPath.c_str());
              fs::create_directories(filename.parent_path());
              f = fopen(fullPath.c_str(), "w+");
            }
            const auto blobSize = git_blob_rawsize(blob);
            const auto writeCount = blobSize == 0 ? 0 : 1;
            if (fwrite(git_blob_rawcontent(blob), blobSize, writeCount, f) != writeCount) {
              response.builder().getStatus().setCode(1);
              response.builder().getStatus().setMessage(
                strCat("Failed to reset file ", fullPath.c_str()));
              break;
            }
            fclose(f);
            chmod(fullPath.c_str(), git_tree_entry_filemode(entry));
          }
          file.setStatus(G8FileInfo::Status::UNMODIFIED);
          file.setDirty(false);
          processed.insert(std::make_pair(path, file));

        } else if (type == GIT_OBJECT_TREE) {
          response.builder().getStatus().setCode(1);
          response.builder().getStatus().setMessage(
            strCat("Directory revert is not currently supported for %s", path.c_str()));
          break;
        }
      }

      // post-checkout
      auto postCheckoutRes = postCheckoutHook(
        repo, git_reference_name(client), git_reference_name(client), CheckoutType::FILE);
      if (postCheckoutRes && *postCheckoutRes != 0) {
        // TODO(pawel) report non-zero exit status?
      }

      // Even on errors we would like to come here so that we can return the processed list
      response.builder().getFiles().initInfo(processed.size());
      int i{};
      for (auto [path, file] : processed) {
        response.builder().getFiles().getInfo().setWithCaveats(i++, file);
      }

      break;
    }
    case G8FileRequest::Action::LIST: {
      Vector<fs::path> paths;

      fs::path repoPath = req.getRepo().cStr();

      for (const auto &pathReader : req.getPaths()) {
        paths.emplace_back(pathReader.cStr());
      }

      GIT_PTR(index, index);
      CHECK_GIT(git_repository_index(&index, repo));

      Vector<std::unique_ptr<MutableRootMessage<G8FileInfo>>> foundFiles;

      // Lambda to say whether an index entry is a skip-worktree file.
      auto isSkipWorktree = [](const git_index_entry *entry) {
        return (entry->flags_extended & GIT_INDEX_ENTRY_SKIP_WORKTREE);
      };

      // Go through each file and query for it in the filesystem and git index.
      for (const auto &filename : paths) {
        const git_index_entry *entry = nullptr;

        fs::path fullPath = fs::weakly_canonical(repoPath / filename);

        bool foundOnWorkdir = fs::exists(fullPath);

        if (foundOnWorkdir) {
          struct stat fileStat;
          lstat(fullPath.c_str(), &fileStat);

          if (!S_ISDIR(fileStat.st_mode)) {
            // This is not a directory, so we can add it to the output.
            addWorkdirEntryToOutput(fileStat, fullPath, filename.c_str(), nullptr, foundFiles);
            // We are done, since we found the path as a file in the working directory.
            continue;
          }
        } else {
          entry = git_index_get_bypath(index, filename.c_str(), GIT_INDEX_STAGE_NORMAL);
          if (entry) {
            // Add the index entry file to the output if it is a skip-worktree file.
            if (isSkipWorktree(entry)) {
              addIndexEntryToOutput(repo, filename.c_str(), nullptr, entry, foundFiles);
            }
            // We are done, since we found the path as a file in the index.
            continue;
          }
        }

        auto isVisibleFile = [&req](const fs::path &path) {
          return req.getIncludeDotFiles() || !path.filename().native().starts_with(".");
        };

        // Keep a map of the directories we find in the workdir so we can match them up with the
        // index.
        TreeMap<fs::path, G8FileInfo::Builder> workdirDirMap;

        std::optional<G8FileInfo::Builder> dotFile;
        std::optional<G8FileInfo::Builder> doubleDotFile;

        if (foundOnWorkdir) {
          // We have a directory on the filesystem. Iterate through it, adding entries to the
          // output.
          for (auto const &workdirEntry : fs::directory_iterator(fullPath)) {
            if (isVisibleFile(workdirEntry.path())) {
              struct stat fileStat;
              lstat(workdirEntry.path().c_str(), &fileStat);

              fs::path relativePath = workdirEntry.path().lexically_relative(repoPath);

              // Add the directory entry to the output.
              auto fileInfo = addWorkdirEntryToOutput(
                fileStat, workdirEntry.path(), relativePath.c_str(), filename.c_str(), foundFiles);

              workdirDirMap.emplace(relativePath, fileInfo);
            }
          }
          if (req.getIncludeDotFiles()) {
            // Include '.' in the list of files.
            {
              struct stat fileStat;
              lstat(fullPath.c_str(), &fileStat);

              fs::path relativePath = fullPath.lexically_relative(repoPath) / ".";
              if (relativePath == fs::path("./.")) {
                relativePath = fs::path(".");
              }

              // Include the '.' file in the output.
              dotFile = addWorkdirEntryToOutput(
                fileStat, fs::path(), relativePath.c_str(), filename.c_str(), foundFiles);
              workdirDirMap.emplace(relativePath, dotFile.value());
            }

            // Include '..' in the list of files.
            {
              fs::path parentPath = fullPath.parent_path();
              struct stat fileStat;
              lstat(parentPath.c_str(), &fileStat);

              fs::path relativePath = fullPath.lexically_relative(repoPath) / "..";
              if (relativePath == fs::path("./..")) {
                relativePath = fs::path("..");
              }

              // Include the '..' file in the output.
              doubleDotFile = addWorkdirEntryToOutput(
                fileStat, fs::path(), relativePath.c_str(), filename.c_str(), foundFiles);
              workdirDirMap.emplace(relativePath, doubleDotFile.value());
            }
          }
        }

        // If the path is the top-level directory, we need to prune the
        // represent it as the empty path for prefix matches.
        fs::path dirname = fs::weakly_canonical(filename).lexically_relative(repoPath);

        // If dirname ends in a slash, remove it.
        if (!dirname.empty() && dirname.native().back() == '/') {
          dirname = dirname.parent_path();
        }

        // If dirname is just a dot, make it empty.
        if (dirname == fs::path(".")) {
          dirname = fs::path();
        }

        // Now look through the index for any skip-worktree files or directories.
        size_t start = 0;
        if (!dirname.empty() && (git_index_find_prefix(&start, index, dirname.c_str()) != 0)) {
          // The index doesn't contain any files with this prefix, so continue.
          if (!foundOnWorkdir) {
            auto status = response.builder().getStatus();
            status.setCode(1);
            status.setMessage(format(
              "%sls: %s: No such file or directory\n",
              status.getMessage().cStr(),
              filename.c_str()));
          }
          continue;
        }

        fs::path lastDirectChildPath;
        fs::path lastSubdirectoryPath;
        std::optional<G8FileInfo::Builder> lastDirectChildInfo;

        // Iterate through the index entries, starting at the first one matching the directory
        // prefix.
        while ((entry = git_index_get_byindex(index, start++))) {
          fs::path entryPath(entry->path);

          auto dirPathSegmentIter = dirname.begin();
          auto entryPathSegmentIter = entryPath.begin();

          // Determine if entryPath is a subdirectory of dirname.
          bool isSubpath = true;
          while (dirPathSegmentIter != dirname.end() && entryPathSegmentIter != entryPath.end()) {
            if (*dirPathSegmentIter != *entryPathSegmentIter) {
              isSubpath = false;
            }
            ++dirPathSegmentIter;
            ++entryPathSegmentIter;
          }

          if (!isSubpath) {
            // The entry is not a subpath of the directory we are traversing, meaning we have
            // traversed all children.
            break;
          }

          // Compute the hierarchy path depth of the entry relative to the directory we are
          // traversing.
          auto dirnameLength = std::distance(dirname.begin(), dirname.end());
          auto pathDepth = std::distance(entryPath.begin(), entryPath.end()) - dirnameLength;

          if (pathDepth == 1) {
            // With pathDepth==1, this is a direct child of the directory we are traversing.
            if (workdirDirMap.contains(entryPath)) {
              // The file is in the workdir, so we can ignore the index entry.
              continue;
            }
            if (isSkipWorktree(entry)) {
              if (isVisibleFile(entryPath)) {
                // If this is a skip-worktree file in the index, add it to the list of files.
                addIndexEntryToOutput(repo, entryPath.c_str(), filename.c_str(), entry, foundFiles);
              }

              if (req.getIncludeDotFiles()) {
                if (!dotFile.has_value()) {
                  // We need to add the "." entry to the output if including hidden files and it
                  // doesn't already exist.
                  fs::path relativePath = fullPath.lexically_relative(repoPath) / ".";
                  if (relativePath == fs::path("./.")) {
                    relativePath = fs::path(".");
                  }

                  dotFile = addInferredDirectoryIndexEntryToOutput(
                    relativePath.c_str(), filename.c_str(), 2, entry, foundFiles);
                }

                if (!doubleDotFile.has_value()) {
                  // We need to add the ".." entry to the output if including hidden files and it
                  // doesn't already exist.
                  fs::path relativePath = fullPath.lexically_relative(repoPath) / "..";
                  if (relativePath == fs::path("./..")) {
                    relativePath = fs::path("..");
                  }
                  doubleDotFile = addInferredDirectoryIndexEntryToOutput(
                    relativePath.c_str(), filename.c_str(), 2, entry, foundFiles);
                }
              }

              // Increment the link count and byte size of the "." directory to account for this
              // skip-worktree file.
              if (dotFile.has_value()) {
                dotFile->setLinkCount(dotFile->getLinkCount() + 1);
                dotFile->setByteSize(dotFile->getByteSize() + 32);
              }
            }
          } else {
            // With pathDepth > 1, we are looking at a file in a recursive subdirectory. Here we
            // loop to find the pathDepth=1 path and pathDepth=2 path.
            fs::path directoryPath = entryPath;
            fs::path subdirectoryPath = entryPath;

            while (pathDepth > 2) {
              subdirectoryPath = subdirectoryPath.parent_path();
              directoryPath = directoryPath.parent_path();
              --pathDepth;
            }

            while (pathDepth > 1) {
              directoryPath = directoryPath.parent_path();
              --pathDepth;
            }

            if (
              isSkipWorktree(entry) && !workdirDirMap.contains(directoryPath)
              && directoryPath != lastDirectChildPath) {
              // Increment the link count for the "." directory if we have a new skip-worktree
              // child.
              if (dotFile.has_value()) {
                dotFile->setLinkCount(dotFile->getLinkCount() + 1);
                dotFile->setByteSize(dotFile->getByteSize() + 32);
              }
            }

            // We will need to increment the link count for the directory, if this is the first
            // time we have encountered a new skip-worktree index entry for a direct child of our
            // directory.
            bool incrementLinkCount = false;
            if (
              isSkipWorktree(entry) && subdirectoryPath != lastSubdirectoryPath
              && !fs::exists(repoPath / subdirectoryPath)) {
              incrementLinkCount = true;
            }
            lastSubdirectoryPath = subdirectoryPath;

            if (workdirDirMap.contains(directoryPath)) {
              // The directory is in the workdir, so all we need to do is bump the link count for
              // any direct children with skip-worktree set.
              if (incrementLinkCount) {
                auto workFileInfo = workdirDirMap.at(directoryPath);
                workFileInfo.setLinkCount(workFileInfo.getLinkCount() + 1);
                workFileInfo.setByteSize(workFileInfo.getByteSize() + 32);
              }
              lastDirectChildPath = directoryPath;
              continue;
            } else {
              // If the index entry is not a skip-worktree file, we should assume it was deleted
              // as the workdir doesn't contain its pathDepth=1 parent.
              if (!isSkipWorktree(entry)) {
                continue;
              }
            }

            // If we made it this far, there is no pathDepth=1 directory in the working dir,
            // and we have a skip-worktree index entry.
            if (lastDirectChildPath != directoryPath) {
              // This is a new skip-worktree index entry corresponding to a child or sub-child
              // of the directory we are traversing so we need to add the directory to the
              // output.
              if (isVisibleFile(directoryPath)) {
                // Include this directory in the output.
                lastDirectChildInfo = addInferredDirectoryIndexEntryToOutput(
                  directoryPath.c_str(), filename.c_str(), 3, entry, foundFiles);
                lastDirectChildPath = directoryPath;
              }

              if (req.getIncludeDotFiles()) {
                if (!dotFile.has_value()) {
                  // We need to add the "." entry to the output if including hidden files and it
                  // doesn't already exist.
                  fs::path relativePath = fullPath.lexically_relative(repoPath) / ".";
                  if (relativePath == fs::path("./.")) {
                    relativePath = fs::path(".");
                  }

                  dotFile = addInferredDirectoryIndexEntryToOutput(
                    relativePath.c_str(), filename.c_str(), 3, entry, foundFiles);
                }

                if (!doubleDotFile.has_value()) {
                  // We need to add the ".." entry to the output if including hidden files and it
                  // doesn't already exist.
                  fs::path relativePath = fullPath.lexically_relative(repoPath) / "..";
                  if (relativePath == fs::path("./..")) {
                    relativePath = fs::path("..");
                  }

                  doubleDotFile = addInferredDirectoryIndexEntryToOutput(
                    relativePath.c_str(), filename.c_str(), 2, entry, foundFiles);
                }
              }
            } else {
              // This is the same as the last directory we added, so all we have to do is bump the
              // link count.
              if (incrementLinkCount) {
                lastDirectChildInfo->setLinkCount(lastDirectChildInfo->getLinkCount() + 1);
                lastDirectChildInfo->setByteSize(lastDirectChildInfo->getByteSize() + 32);
              }
            }
          }
        }
      }

      response.builder().getFiles().initInfo(foundFiles.size());
      int i = 0;
      for (const auto &file : foundFiles) {
        response.builder().getFiles().getInfo().setWithCaveats(i++, file->builder());
      }

      break;
    }
  }

  return response;
}

}  // namespace c8
