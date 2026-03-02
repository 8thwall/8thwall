def _unity_platform_transition_impl(settings, attr):
    # Store the original target platform from the command line in a new 'original-platform' flag.
    original_target_platform = settings.get("//bzl/unity/impl:original-platform")
    if original_target_platform == Label("//bzl/unity/impl:unset-platform"):
        original_target_platform = settings.get("//command_line_option:platforms", Label("//bzl/unity/impl:unset-platform"))[0]

    # 1:1 transition to build with a 'unity' platform.
    previous_platforms = settings.get("//command_line_option:platforms", None)
    if not previous_platforms or str(previous_platforms[0]).find("@unity-version//:") == -1:
        # Choose the default unity platform.
        return {
            "//command_line_option:platforms": [attr._unity_platform],
            "//bzl/unity/impl:original-platform": original_target_platform,
        }
    else:
        # Propagate the existing unity platform.
        return {
            "//command_line_option:platforms": previous_platforms,
            "//bzl/unity/impl:original-platform": original_target_platform,
        }

unity_platform_transition = transition(
    implementation = _unity_platform_transition_impl,
    inputs = ["//command_line_option:platforms", "//bzl/unity/impl:original-platform"],
    outputs = ["//command_line_option:platforms", "//bzl/unity/impl:original-platform"],
)

def _unity_target_arch_transition_impl(settings, attr):
    # Store the original target platform from the command line in a new 'original-platform' flag.
    original_target_platform = settings.get("//bzl/unity/impl:original-platform")
    if original_target_platform == Label("//bzl/unity/impl:unset-platform"):
        original_target_platform = settings.get("//command_line_option:platforms", Label("//bzl/unity/impl:unset-platform"))[0]

    # 1:1 transition to build a specified architecture (e.g., android_arm64) with a 'unity' platform.
    previous_platforms = settings.get("//command_line_option:platforms", None)
    current_platform = previous_platforms[0]
    if current_platform == Label("@local_config_platform//:host"):
        # Rewrite host target platform to unity target platform.
        return {
            "//command_line_option:platforms": ["@unity-version//:host-unity-local-platform"],
        }
    elif str(current_platform).find("unity-version//:") == -1:
        # Rewrite target platform to unity target platform.
        new_platform = "@unity-version//:{target_platform}-unity-local-platform".format(target_platform = current_platform.name)
        return {
            "//command_line_option:platforms": new_platform,
            "//bzl/unity/impl:original-platform": original_target_platform,
        }
    else:
        return {
            "//command_line_option:platforms": previous_platforms,
            "//bzl/unity/impl:original-platform": original_target_platform,
        }

unity_target_arch_transition = transition(
    implementation = _unity_target_arch_transition_impl,
    inputs = ["//command_line_option:platforms", "//bzl/unity/impl:original-platform"],
    outputs = ["//command_line_option:platforms", "//bzl/unity/impl:original-platform"],
)

def _unity_target_platforms_transition_impl(settings, attr):
    # 1:2+ transition to build deps for each target platform.
    platforms = {}
    original_platform = settings.get("//bzl/unity/impl:original-platform", Label("@local_config_platform//:host"))
    for platform in attr.target_platforms:
        if not attr.build_all_target_platforms and platform != original_platform:
            # Skip platforms that don't match the original_platform if build_all_target_platforms is False.
            continue
        platforms[platform.name] = {
            "//command_line_option:platforms": [platform],
            "//bzl/unity/impl:original-platform": Label("//bzl/unity/impl:unset-platform"),
        }
    return platforms

unity_target_platforms_transition = transition(
    implementation = _unity_target_platforms_transition_impl,
    inputs = ["//command_line_option:platforms", "//bzl/unity/impl:original-platform"],
    outputs = ["//command_line_option:platforms", "//bzl/unity/impl:original-platform"],
)
