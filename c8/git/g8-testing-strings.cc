// Copyright (c) 2021 8th Wall, Inc.

#include "bzl/inliner/rules2.h"

cc_library {
  hdrs = {
    "g8-testing-strings.h",
  };
  visibility = {
    "//visibility:private",
  };
  deps = {
    "//c8:string",
  };
}
cc_end(0x5f6d9d4f);

#include "c8/git/g8-testing-strings.h"

namespace c8 {
namespace g8testing {

String configFileA1 = R"--(
[core]
	repositoryformatversion = 0
	filemode = true
	bare = false
	logallrefupdates = true
	ignorecase = true
	precomposeunicode = true
[remote "origin"]
	url = https://github.com/8thwall/code8.git
	fetch = +refs/heads/*:refs/remotes/origin/*
[branch "master"]
	remote = origin
	merge = refs/heads/master
[lfs "https://github.com/8thwall/code8.git/info/lfs"]
	access = basic
[branch "g8-CS939441"]
	remote = origin
	merge = refs/heads/pawel-g8-CS939441
[branch "clean-CS219492"]
	remote = origin
	merge = refs/heads/pawel-clean-CS219492
[branch "studiodeploy-CS578341"]
[branch "studiodeploy-CS572077"]
[branch "studiodeploy-CS436836"]
	remote = origin
	merge = refs/heads/pawel-studiodeploy-CS436836
)--";

String configFileA2 = R"--(
[core]
	repositoryformatversion = 0
	filemode = true
	bare = false
	logallrefupdates = true
	ignorecase = true
	precomposeunicode = true
[remote "origin"]
	url = https://github.com/8thwall/code8.git
	fetch = +refs/heads/*:refs/remotes/origin/*
[branch "master"]
	remote = origin
	merge = refs/heads/master
[lfs "https://github.com/8thwall/code8.git/info/lfs"]
	access = basic
[branch "g8-CS939441"]
	remote = origin
	merge = refs/heads/pawel-g8-CS939441
[branch "clean-CS219492"]
	remote = origin
	merge = refs/heads/pawel-clean-CS219492
[branch "studiodeploy-CS436836"]
	remote = origin
	merge = refs/heads/pawel-studiodeploy-CS436836
)--";

}  // namespace g8testing
}  // namespace c8
