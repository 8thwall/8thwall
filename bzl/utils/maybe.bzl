load("@bazel_tools//tools/build_defs/repo:utils.bzl", "maybe")

# Wrapper macro for 'https://bazel.build/rules/lib/repo/utils#maybe' that allows
# one to introspect the context of the existing external repository (if present)
def niantic_maybe(repo_rule, name, verbose = False, **kwargs):
    if verbose:
        res = native.existing_rule(name)
        if res:
            print("Target repository with name '{}' already exists; skipping.".format(name))
            print("Existing '{}' context:".format(name))
            for k, v in res.items():
                print("'{}' => '{}'".format(k, v))
        else:
            print("No repository with name '{}' present yet; Adding the repository now.".format(name))
    maybe(repo_rule, name, **kwargs)
