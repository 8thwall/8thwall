# This cached file can be used for tests, since it needs to live outside
# of the gazelle sandbox.
# One must create a genrule in order to add it as a data dep of a test.
# Example:
#genrule(
#    name = "proj_db_outside_gazelle",
#    srcs = ["//third_party/proj:proj.db"],
#    outs = ["proj.db"],
#    cmd = "cp $(location //third_party/proj:proj.db) $(OUTS)",
#)

exports_files(
    [
        "proj.db",
    ],
)