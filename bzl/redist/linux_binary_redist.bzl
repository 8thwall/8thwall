def _impl(ctx):
    # Get the target binary file from the dependencies
    binary_executable_filepath = ctx.file.binary_executable.path

    # Generate the output file path
    out_file = ctx.actions.declare_file(ctx.attr.name)

    output_libstdcpp = ctx.actions.declare_file("libstdc++.so.6")
    output_libgcc_s = ctx.actions.declare_file("libgcc_s.so.1")

    redist_libraries = [
        output_libstdcpp,
        output_libgcc_s,
    ]

    all_stdcpp_lib = " ".join([ll.path for ll in ctx.files._libstdcpp])

    # Copy the binary to the new location
    ctx.actions.run_shell(
        inputs = [
            ctx.file._linux_binary_redist,
            ctx.file.binary_executable,
            ctx.file._ldd,
            ctx.file._libgcc_s,
        ],
        outputs = [out_file] + redist_libraries,
        command = "{linux_binary_redist_sh}".format(
            linux_binary_redist_sh = ctx.file._linux_binary_redist.path,
        ),
        env = {
            "BINARY_EXECUTABLE": binary_executable_filepath,
            "REDIST_BINARY_EXECUTABLE": out_file.path,
            "HERMETIC_LDD": ctx.file._ldd.path,
            "HERMETIC_LIBGCC_S": ctx.file._libgcc_s.path,
            "HERMETIC_LIBSTDCPP": all_stdcpp_lib,
            "OUTPUT_LIBSTDCPP": output_libstdcpp.path,
            "OUTPUT_LIBGCC_S": output_libgcc_s.path,
        },
        execution_requirements = {
            "local": "1",
        },
    )

    return [DefaultInfo(executable = out_file, files = depset([out_file, output_libstdcpp, output_libgcc_s]))]

# Define the rule
linux_redist = rule(
    implementation = _impl,
    attrs = {
        "binary_executable": attr.label(
            mandatory = True,
            allow_single_file = True,
            cfg = "exec",
        ),
        "_ldd": attr.label(
            allow_single_file = True,
            cfg = "exec",
            default = "//bzl/crosstool:hermetic_ldd",
        ),
        "_libgcc_s": attr.label(
            allow_single_file = True,
            cfg = "exec",
            default = "//bzl/crosstool:hermetic_libgcc_s",
        ),
        "_libstdcpp": attr.label_list(
            cfg = "exec",
            default = ["//bzl/crosstool:hermetic_libstdc++"],
        ),
        "_linux_binary_redist": attr.label(
            default = Label(":linux_binary_redist"),
            allow_single_file = True,
            executable = True,
            cfg = "exec",
        ),
    },
)
