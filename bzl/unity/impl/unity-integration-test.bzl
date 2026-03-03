load("@rules_python//python:defs.bzl", "current_py_toolchain")
load("//bzl/unity/impl:transitions.bzl", "unity_platform_transition")
load("//bzl/unity/impl:unity-app.bzl", "copy_action")

def _unity_integration_tests_impl(ctx):
    # Copy all project files to build directory.
    # Because Bazel has a limitation where spaces can't exist for runfiles,
    # we sanitize the runfiles filenames to not have spaces
    sanitized_runfiles = []
    pathStart = len(ctx.attr.project[0].files_path_prefix) + 1
    for src_in in ctx.attr.project[0].unity_srcs:
        src_out_name = "%s/%s" % (ctx.label.name, src_in.path[pathStart:])
        if " " in src_out_name:
            src_out_name = src_out_name.replace(" ", "")
        src_out = ctx.actions.declare_file(src_out_name)
        print("Copying %s to %s" % (src_in.path, src_out.path))
        copy_action(ctx, src_in, src_out)
        sanitized_runfiles.append(src_out)

    # In order to make sure we are running the tests in right version of unity
    # we will open and close the project in the specified unity version of the
    # project. This is to run tests in a unity version that is different from
    # the source project's target unity version
    files_path_prefix = "%s/%s/%s" % (ctx.configuration.bin_dir.path, ctx.label.package, ctx.label.name)
    relative_project_path = "%s/%s" % (
        files_path_prefix,
        ctx.attr.project[0].project_relative_path,
    )
    full_project_path = "$WORKSPACE/%s" % (
        relative_project_path,
    )
    unity_preparation_log_filename = "%s_unity_integration_test_preperation_log.txt" % (ctx.label.name)
    unity_preparation_log_path = "%s/%s/%s" % (
        ctx.bin_dir.path,
        ctx.label.package,
        unity_preparation_log_filename,
    )
    unity_preparation_log = ctx.actions.declare_file(unity_preparation_log_filename)
    print("Unity preparation log at: %s" % unity_preparation_log_path)
    build_args = [
        "-batchmode",
        "-nographics",
        "-upmNoDefaultPackages",
        "-projectPath", full_project_path,
        "-logFile", unity_preparation_log_path,
        "--quit",
    ]
    ctx.actions.run(
        inputs = sanitized_runfiles,
        executable = ctx.executable.unitybuild,
        arguments = build_args,
        outputs = [unity_preparation_log],
        tools = ctx.files._python,
        env = {
            "WORKSPACE_ENV": ctx.file._workspace_env.path,
            "UNITY": ctx.attr.project[0].unity_toolchain.unityinfo.unity_path,
            "PYTHON3": ctx.attr._python[platform_common.ToolchainInfo].py3_runtime.interpreter.path,
        },
        progress_message = "Running Unity Preparation with {}".format(str(ctx.executable.unitybuild.path)),
        execution_requirements = {
            "no-cache": "1",
            "local": "1",
        },
    )

    test_runner_path = "%s/%s" % (ctx.label.name, "test-runner")
    test_runner = ctx.actions.declare_file(test_runner_path)

    # Need to determine the build target the unity editor should open with
    host_constraints = ctx.attr._host_platform[platform_common.PlatformInfo].constraints
    osx_constraint_values = ctx.attr._os_osx[platform_common.ConstraintValueInfo]
    windows_constraint_values = ctx.attr._os_windows[platform_common.ConstraintValueInfo]
    is_host_osx = host_constraints.has_constraint_value(osx_constraint_values)
    is_host_windows = host_constraints.has_constraint_value(windows_constraint_values)
    if is_host_osx:
        platform = "osx"
    elif is_host_windows:
        platform = "win64"
    else:
        # Open editor in the same platform as the testPlatform if not desktop
        platform = ctx.attr.mode

    ctx.actions.write(
        test_runner,
        content = (
            """#!/bin/bash\n""" +
            """set -u\n""" +
            """export HOME="$PWD/_home"\n""" +
            # Check if UNITY_EXECUTABLE environment variable is set - if so use that as unity executable
            """UNITY=${UNITY_EXECUTABLE:-%s}\n""" % (ctx.attr.project[0].unity_toolchain.unityinfo.unity_path) +
            # Check if EXTERNAL_CATEGORIES environment variable is set otherwise, just empty
            """EXTERNAL_CATEGORIES=${EXTERNAL_CATEGORIES:-}\n""" +
            # Possibility to override outputs
            """RUN_LOG=${RUN_LOG:-run.log}\n""" +
            """TESTRESULTS_XML=${TESTRESULTS_XML:-testresults.xml}\n""" +
            """PROJECT_PATH=$PWD/%s/%s/%s\n""" % (ctx.attr.project[0].build_file_path, ctx.label.name, ctx.attr.project[0].project_relative_path) +
            """TEST_COMMAND="\\"$UNITY\\" -runTests -batchmode """ +
            """-projectPath $PROJECT_PATH -testResults $PWD/$TESTRESULTS_XML """ +
            """-testPlatform %s -logFile $RUN_LOG """ % (ctx.attr.mode) +
            """-buildTarget %s """ % (platform) +
            """-testCategory %s$EXTERNAL_CATEGORIES """ % (ctx.attr.categories) +
            """-upmNoDefaultPackages;"\n""" +
            """echo "-- Running the following"\n""" +
            """echo $TEST_COMMAND;\n""" +
            # 'status=$?' captures the exit code of the unity editor running tests. returns 0 when pass, non-zero when failed.
            # Please make sure that this status=$? is always run after the unity command as we need to save the result of it before executing anything else
            # or bazel could be thinking the tests pass based on the success or failure of some other command that isnt the test
            # Also, do not capture 'status' in 'eval $TEST_COMMAND` but after `eval $TEST_COMMAND`.
            """eval $TEST_COMMAND; status=$?\n""" +
            """echo "-- Unity exit status: $status --"\n""" +
            """cat $RUN_LOG\n""" +
            """if [ ! -f $TESTRESULTS_XML ]; then\n""" +
            """  echo "Test result file $TESTRESULTS_XML not generated. In order for the test to be considered successful, $TESTRESULTS_XML needs to be present at $PWD"\n""" +
            """  exit 1\n""" +
            """fi\n""" +
            """echo "-- Contents of $TESTRESULTS_XML"\n""" +
            """cat $TESTRESULTS_XML\n""" +
            """printf "\n\n"\n""" +
            """echo "-- Unity exit status was: $status"\n""" +
            """exit $status"""
        ),
        is_executable = True,
    )

    return [DefaultInfo(executable = test_runner, runfiles = ctx.runfiles(sanitized_runfiles), files = depset([unity_preparation_log]))]

unity_integration_test = rule(
    implementation = _unity_integration_tests_impl,
    executable = True,
    test = True,
    attrs = {
        "_allowlist_function_transition": attr.label(
            default = "@bazel_tools//tools/allowlists/function_transition_allowlist",
        ),
        "project": attr.label(
            cfg = unity_platform_transition,
            mandatory = True,
            providers = ["unity_srcs", "unity_toolchain"],
        ),
        "mode": attr.string(
            default = "EditMode",
        ),
        "categories": attr.string(
            default = "!SkipOnCi\\;!RequiresNetworkConnection",
        ),
        "unitybuild": attr.label(
            default = Label("//bzl/unity:unity-build"),
            allow_single_file = True,
            executable = True,
            cfg = "host",
        ),
        "_workspace_env": attr.label(
            default = "@workspace-env//:workspace-env",
            allow_single_file = True,
        ),
        "_python": attr.label(
            default = Label("@rules_python//python:current_py_toolchain"),
            providers = [platform_common.ToolchainInfo],
            cfg = "exec",
        ),
        "_unity_platform": attr.label(
            default = "@unity-version//:local",
        ),
        "_os_osx": attr.label(
            default = "@platforms//os:osx",
            providers = [platform_common.ConstraintValueInfo],
        ),
        "_os_windows": attr.label(
            default = "@platforms//os:windows",
            providers = [platform_common.ConstraintValueInfo],
        ),
        "_host_platform": attr.label(
            default = "@local_config_platform//:host",
            providers = [platform_common.PlatformInfo],
        ),
    },
)
