licenses(["notice"])  # Apache v2; Ref: https://github.com/oneapi-src/oneTBB/blob/v2020.3/LICENSE

TBB_SOURCES = [
    "src/rml/client/rml_tbb.cpp",
    "src/tbb/arena.cpp",
    "src/tbb/cache_aligned_allocator.cpp",
    "src/tbb/concurrent_hash_map.cpp",
    "src/tbb/concurrent_monitor.cpp",
    "src/tbb/concurrent_queue.cpp",
    "src/tbb/concurrent_vector.cpp",
    "src/tbb/condition_variable.cpp",
    "src/tbb/critical_section.cpp",
    "src/tbb/dynamic_link.cpp",
    "src/tbb/governor.cpp",
    "src/tbb/itt_notify.cpp",
    "src/tbb/market.cpp",
    "src/tbb/mutex.cpp",
    "src/tbb/observer_proxy.cpp",
    "src/tbb/pipeline.cpp",
    "src/tbb/private_server.cpp",
    "src/tbb/queuing_mutex.cpp",
    "src/tbb/queuing_rw_mutex.cpp",
    "src/tbb/reader_writer_lock.cpp",
    "src/tbb/recursive_mutex.cpp",
    "src/tbb/scheduler.cpp",
    "src/tbb/semaphore.cpp",
    "src/tbb/spin_mutex.cpp",
    "src/tbb/spin_rw_mutex.cpp",
    "src/tbb/task.cpp",
    "src/tbb/task_group_context.cpp",
    "src/tbb/tbb_main.cpp",
    "src/tbb/tbb_misc.cpp",
    "src/tbb/tbb_misc_ex.cpp",
    "src/tbb/tbb_statistics.cpp",
    "src/tbb/tbb_thread.cpp",
    "src/tbb/x86_rtm_rw_mutex.cpp",
]

genrule(
    name = "version_string",
    outs = ["version_string.h"],
    cmd = "echo '#define TBB_MANIFEST 1' >> $@",
)

cc_library(
    name = "tbb",
    srcs = TBB_SOURCES + glob([
        "src/tbb/*.h",
        "include/tbb/internal/*.h",
    ]) + [
        "src/rml/client/rml_factory.h",
        "src/rml/server/thread_monitor.h",
        "src/rml/include/rml_tbb.h",
        "src/rml/include/rml_base.h",
    ] + [
        ":version_string",
    ],
    hdrs = glob([
        "include/serial/tbb/*.h",
        "include/tbb/*.h",
        "include/tbb/compat/*",
        "include/tbb/machine/*.h",
    ]),
    copts = [
        "-std=c++17",  # Test sources make use of functionality deprecated in C++20
        "-mrtm",
    ],
    defines = select({
        "@platforms//os:windows": [],
        "//conditions:default": [
            "USE_PTHREAD",  # Propagate this to consumers to avoid falling back to using windows API
        ],
    }),
    includes = [
        "include",
        "src",
    ],
    linkopts = select({
        "@platforms//os:windows": [],
        "//conditions:default": [
            "-lpthread",
        ],
    }),
    visibility = ["//visibility:public"],
    # Omit integration of Intel ITT API for x86-64 builds
)

TEST_DRIVER_REL_PATH = "src/test"

# TODO(peter) A number of the tests contain includes of the pattern
# '#include <*.cpp>', which can sometimes be mitigated via the use
# of 'textual_hdrs'. However, this lead to issues during linking that
# involve duplicate symbols. For now, these tests are disabled.
TEST_DRIVER_FILES = [
    "test_aggregator.cpp",
    "test_aligned_space.cpp",
    # "test_assembly_compiler_builtins.cpp",
    "test_async_msg.cpp",
    "test_async_node.cpp",
    "test_atomic.cpp",
    # "test_atomic_compiler_builtins.cpp",
    "test_atomic_pic.cpp",
    "test_blocked_range.cpp",
    "test_blocked_range2d.cpp",
    "test_blocked_range3d.cpp",
    "test_blocked_rangeNd.cpp",
    "test_broadcast_node.cpp",
    "test_buffer_node.cpp",
    "test_cache_aligned_allocator.cpp",
    "test_cache_aligned_allocator_STL.cpp",
    "test_combinable.cpp",
    "test_composite_node.cpp",
    "test_concurrent_hash_map.cpp",
    "test_concurrent_lru_cache.cpp",
    "test_concurrent_map.cpp",
    # "test_concurrent_monitor.cpp",
    "test_concurrent_priority_queue.cpp",
    "test_concurrent_queue.cpp",
    "test_concurrent_set.cpp",
    "test_concurrent_unordered_map.cpp",
    "test_concurrent_unordered_set.cpp",
    "test_concurrent_vector.cpp",
    "test_continue_node.cpp",
    "test_critical_section.cpp",
    # "test_dynamic_link.cpp",
    "test_eh_algorithms.cpp",
    "test_eh_flow_graph.cpp",
    "test_eh_tasks.cpp",
    # "test_enumerable_thread_specific.cpp",
    # "test_examples_common_utility.cpp",
    "test_flow_graph.cpp",
    "test_flow_graph_whitebox.cpp",
    "test_fp.cpp",
    "test_function_node.cpp",
    "test_global_control.cpp",
    "test_halt.cpp",
    "test_handle_perror.cpp",
    "test_hw_concurrency.cpp",
    "test_indexer_node.cpp",
    "test_inits_loop.cpp",
    "test_input_node.cpp",
    "test_intrusive_list.cpp",
    "test_ittnotify.cpp",
    "test_join_node.cpp",
    "test_join_node_key_matching.cpp",
    "test_join_node_msg_key_matching.cpp",
    "test_lambda.cpp",
    "test_limiter_node.cpp",
    "test_model_plugin.cpp",
    "test_multifunction_node.cpp",
    "test_mutex.cpp",
    "test_mutex_native_threads.cpp",
    "test_overwrite_node.cpp",
    "test_parallel_do.cpp",
    "test_parallel_for.cpp",
    "test_parallel_for_each.cpp",
    "test_parallel_for_vectorization.cpp",
    "test_parallel_invoke.cpp",
    "test_parallel_pipeline.cpp",
    "test_parallel_reduce.cpp",
    "test_parallel_scan.cpp",
    "test_parallel_sort.cpp",
    "test_parallel_while.cpp",
    "test_partitioner_whitebox.cpp",
    "test_pipeline.cpp",
    "test_pipeline_with_tbf.cpp",
    "test_priority_queue_node.cpp",
    "test_queue_node.cpp",
    "test_reader_writer_lock.cpp",
    "test_resumable_tasks.cpp",
    "test_rwm_upgrade_downgrade.cpp",
    # "test_semaphore.cpp",
    "test_sequencer_node.cpp",
    "test_source_node.cpp",
    "test_split_node.cpp",
    "test_static_assert.cpp",
    # "test_std_thread.cpp",
    "test_tagged_msg.cpp",
    "test_task.cpp",
    "test_task_arena.cpp",
    "test_task_auto_init.cpp",
    "test_task_enqueue.cpp",
    "test_task_group.cpp",
    "test_task_priority.cpp",
    "test_task_scheduler_init.cpp",
    "test_task_scheduler_observer.cpp",
    "test_task_steal_limit.cpp",
    "test_tbb_condition_variable.cpp",
    "test_tbb_fork.cpp",
    "test_tbb_header.cpp",
    "test_tbb_thread.cpp",
    "test_tbb_version.cpp",
    "test_tick_count.cpp",
    "test_tuple.cpp",
    "test_write_once_node.cpp",
    "test_yield.cpp",
]

[
    cc_test(
        name = test_driver_file.split(".")[0],
        srcs = glob([
            "{}/*.h".format(TEST_DRIVER_REL_PATH),
            TEST_DRIVER_REL_PATH + "/" + test_driver_file,
            "src/tbb/*.h",
        ]) + TBB_SOURCES,
        copts = [
            "-std=c++17",  # Test sources make use of functionality deprecated in C++20
        ],
        includes = [
            TEST_DRIVER_REL_PATH,
            "src",
        ],
        linkopts = select({
            "@platforms//os:windows": [],
            "//conditions:default": [
                "-lpthread",
            ],
        }),
        local_defines = [
            "TEST_USES_TBB",
        ],
        deps = [
            ":tbb",
        ],
    )
    for test_driver_file in TEST_DRIVER_FILES
]
