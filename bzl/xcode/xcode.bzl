load("//bzl/xcode/impl:xcode_app.bzl", _xcode_app = "xcode_app")
load("//bzl/xcode/impl:xcode_project.bzl", _XcodeProjectInfo = "XcodeProjectInfo", _xcode_project = "xcode_project")

XcodeProjectInfo = _XcodeProjectInfo

xcode_app = _xcode_app
xcode_project = _xcode_project
