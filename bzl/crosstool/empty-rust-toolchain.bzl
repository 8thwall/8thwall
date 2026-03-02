# NOTE(christoph): This is a hack to delay the failure to the analysis phase,
# because target_compatible_with doesn't prevent bazel from trying to resolve the Rust toolchain
# even if it wouldn't be used by anything.
# Instead it will fail at build time if the target is not excluded using target_compatible_with.
def _empty_rust_toolchain_impl(ctx):
    # https://github.com/bazelbuild/rules_rust/blob/fe610dab73c3c61560a547bc661177dd5115fed7/rust/toolchain.bzl#L602
    return [
        platform_common.ToolchainInfo(
            all_files = depset([]),
            binary_ext = "empty_rust_toolchain",
            cargo = None,
            clippy_driver = None,
            compilation_mode_opts = {},
            crosstool_files = depset([]),
            default_edition = "empty_rust_toolchain",
            dylib_ext = None,
            env = None,
            exec_triple = None,
            libstd_and_allocator_ccinfo = None,
            libstd_and_global_allocator_ccinfo = None,
            nostd_and_global_allocator_cc_info = None,
            llvm_cov = None,
            llvm_profdata = None,
            make_variables = None,
            rust_doc = None,
            rust_std = None,
            rust_std_paths = [],
            rustc = None,  # ctx.file._error_if_used,
            rustc_lib = None,
            rustfmt = None,
            staticlib_ext = None,
            stdlib_linkflags = None,
            extra_rustc_flags = None,
            extra_exec_rustc_flags = None,
            per_crate_rustc_flags = None,
            sysroot = None,
            sysroot_short_path = None,
            target_arch = None,
            target_flag_value = None,
            target_json = None,
            target_os = None,
            target_triple = None,

            # Experimental and incompatible flags
            _rename_first_party_crates = None,
            _third_party_dir = None,
            _pipelined_compilation = None,
            _experimental_use_cc_common_link = None,
            _experimental_use_global_allocator = None,
            _experimental_use_coverage_metadata_files = None,
            _experimental_toolchain_generated_sysroot = None,
            _incompatible_test_attr_crate_and_srcs_mutually_exclusive = None,
            _no_std = None,
        ),
    ]

empty_rust_toolchain = rule(
    implementation = _empty_rust_toolchain_impl,
)
