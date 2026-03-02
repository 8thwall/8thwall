# This BUILD rules was generated manually by following the logic in
# external/dawn/**/CMakeLists.txt and external/dawn/**/BUILD.gn.
# To upgrade DAWN, diff the new version against the previous commit,
# then manually apply the diffs, converting them from GN to Bazel.

load("@bazel_skylib//lib:selects.bzl", "selects")
load("@bazel_skylib//rules:common_settings.bzl", "string_flag")
load("@the8thwall//bzl/apple:objc.bzl", "cc_or_objc_library", "nia_objc_library")
load("@the8thwall//bzl/node:node-addon.bzl", "node_addon")

cc_library(
    name = "includes",
    hdrs = glob([
        "include/**/*.h",
    ]),
    visibility = ["//visibility:public"],
)

COPTS_APPLE = select({
    "@the8thwall//bzl/conditions:apple": [
        "-DTINT_BUILD_MSL_WRITER",
        "-DDAWN_ENABLE_BACKEND_METAL",
    ],
    "//conditions:default": [],
})

string_flag(
    name = "android-backend",
    build_setting_default = "vulkan",
    values = [
        "gles",
        "vulkan",
    ],
    visibility = ["//visibility:public"],
)
config_setting(
    name = "android-gles",
    flag_values = {
        ":android-backend": "gles",
    },
)
config_setting(
    name = "android-vulkan",
    flag_values = {
        ":android-backend": "vulkan",
    },
)

selects.config_setting_group(
    name = "android-and-gles",
    match_all = ["@the8thwall//bzl/conditions:android", ":android-gles"],
)
selects.config_setting_group(
    name = "android-and-vulkan",
    match_all = ["@the8thwall//bzl/conditions:android", ":android-vulkan"],
)

COPTS_ANDROID = select({
    ":android-and-gles": [
        "-DTINT_BUILD_GLSL_WRITER",
        "-DDAWN_ENABLE_BACKEND_OPENGL",
        "-DDAWN_ENABLE_BACKEND_OPENGLES",
    ],
    ":android-and-vulkan": [
        "-DTINT_BUILD_SPV_READER",
        "-DTINT_BUILD_SPV_WRITER",
        "-DDAWN_ENABLE_BACKEND_VULKAN",
    ],
    "//conditions:default": [],
})

string_flag(
    name = "windows-backend",
    build_setting_default = "d3d12",
    values = [
        "d3d11",
        "d3d12",
    ],
    visibility = ["//visibility:public"],
)
config_setting(
    name = "windows-d3d11",
    flag_values = {
        ":windows-backend": "d3d11",
    },
)
config_setting(
    name = "windows-d3d12",
    flag_values = {
        ":windows-backend": "d3d12",
    },
)

selects.config_setting_group(
    name = "windows-and-d3d11",
    match_all = ["@the8thwall//bzl/conditions:windows", ":windows-d3d11"],
)
selects.config_setting_group(
    name = "windows-and-d3d12",
    match_all = ["@the8thwall//bzl/conditions:windows", ":windows-d3d12"],
)

COPTS_WINDOWS = select({
    ":windows-and-d3d11": [
        "-DDAWN_ENABLE_BACKEND_D3D11",
    ],
    ":windows-and-d3d12": [
        "-DTINT_BUILD_HLSL_WRITER",
        "-DDAWN_ENABLE_BACKEND_D3D12",
    ],
    "//conditions:default": [],
})

string_flag(
    name = "linux-backend",
    build_setting_default = "vulkan",
    values = [
        "gles",
        "gl",
        "vulkan",
    ],
    visibility = ["//visibility:public"],
)
config_setting(
    name = "linux-gles",
    flag_values = {
        ":linux-backend": "gles",
    },
)
config_setting(
    name = "linux-gl",
    flag_values = {
        ":linux-backend": "gl",
    },
)
config_setting(
    name = "linux-vulkan",
    flag_values = {
        ":linux-backend": "vulkan",
    },
)

selects.config_setting_group(
    name = "linux-and-gles",
    match_all = ["@the8thwall//bzl/conditions:linux", ":linux-gles"],
)
selects.config_setting_group(
    name = "linux-and-gl",
    match_all = ["@the8thwall//bzl/conditions:linux", ":linux-gl"],
)
selects.config_setting_group(
    name = "linux-and-vulkan",
    match_all = ["@the8thwall//bzl/conditions:linux", ":linux-vulkan"],
)

COPTS_LINUX = select({
    ":linux-and-gles": [
        "-DTINT_BUILD_GLSL_WRITER",
        "-DDAWN_ENABLE_BACKEND_OPENGL",
        "-DDAWN_ENABLE_BACKEND_OPENGLES",
    ],
    ":linux-and-gl": [
        "-DTINT_BUILD_GLSL_WRITER",
        "-DDAWN_ENABLE_BACKEND_OPENGL",
        "-DDAWN_ENABLE_BACKEND_DESKTOP_GL",
    ],
    ":linux-and-vulkan": [
        "-DTINT_BUILD_SPV_READER",
        "-DTINT_BUILD_SPV_WRITER",
        "-DDAWN_ENABLE_BACKEND_VULKAN",
    ],
    "//conditions:default": [],
})

COMMON_COPTS = [
    "-DDAWN_ENABLE_BACKEND_NULL",
    "-DTINT_BUILD_HLSL_WRITER",
    "-DTINT_BUILD_IR",
    "-DTINT_BUILD_GLSL_VALIDATOR",
    "-DTINT_BUILD_WGSL_READER",
    "-DTINT_BUILD_WGSL_WRITER",
    "-DNODE_ADDON_API_DISABLE_DEPRECATED",
] + COPTS_APPLE + COPTS_ANDROID + COPTS_WINDOWS + COPTS_LINUX

filegroup(
    name = "dawn-webgpu-hdrs",
    srcs = glob(["include/webgpu/*.h"]),
)

cc_library(
    name = "dawn-native-hdrs",
    hdrs = glob(["include/dawn/native/*.h"]),
    visibility = ["//visibility:public"],
)

cc_library(
    name = "dawn-platform-hdrs",
    hdrs = glob(["include/dawn/platform/*.h"]),
    visibility = ["//visibility:public"],
)

cc_library(
    name = "dawn-wire-hdrs",
    hdrs = glob(["include/dawn/wire/*.h"]),
    visibility = ["//visibility:public"],
)

cc_library(
    name = "dawn-hdrs",
    hdrs = [
        "include/dawn/dawn_proc.h",
        "include/dawn/dawn_thread_dispatch_proc.h",
        ":dawn-native-hdrs",
        ":dawn-platform-hdrs",
        ":dawn-wire-hdrs",
    ],
    visibility = [
        "//visibility:public",
    ],
)

cc_library(
    name = "gen-dawn-hdrs",
    srcs = [
        "include/dawn/dawn_proc_table.h",
        "include/dawn/webgpu.h",
        "include/dawn/wire/client/webgpu.h",
    ],
    visibility = ["//visibility:public"],
)

filegroup(
    name = "gen-dawn-cpp-hdrs",
    srcs = [
        "include/dawn/webgpu_cpp.h",
        "include/dawn/webgpu_cpp_print.h",
        "include/dawn/wire/client/webgpu_cpp.h",
        "include/webgpu/webgpu_cpp_chained_struct.h",
    ],
)

filegroup(
    name = "gen-dawn-proc-srcs",
    srcs = [
        "src/dawn/dawn_proc.cpp",
        "src/dawn/dawn_thread_dispatch_proc.cpp",
    ],
)

filegroup(
    name = "gen-webgpu-hdrs",
    srcs = [
        "webgpu-headers/webgpu.h",
    ],
)

filegroup(
    name = "gen-webgpu-dawn-native-proc-srcs",
    srcs = [
        "src/dawn/native/webgpu_dawn_native_proc.cpp",
    ],
)

filegroup(
    name = "gen-dawn-native-utils-hdrs",
    srcs = [
        "src/dawn/native/ChainUtils_autogen.h",
        "src/dawn/native/Features_autogen.h",
        "src/dawn/native/Features_autogen.inl",
        "src/dawn/native/ObjectType_autogen.h",
        "src/dawn/native/ValidationUtils_autogen.h",
        "src/dawn/native/dawn_platform_autogen.h",
        "src/dawn/native/webgpu_absl_format_autogen.h",
        "src/dawn/native/wgpu_structs_autogen.h",
    ],
)

filegroup(
    name = "gen-dawn-native-utils-srcs",
    srcs = [
        "src/dawn/native/ChainUtils_autogen.cpp",
        "src/dawn/native/ObjectType_autogen.cpp",
        "src/dawn/native/ProcTable.cpp",
        "src/dawn/native/ValidationUtils_autogen.cpp",
        "src/dawn/native/webgpu_StreamImpl_autogen.cpp",
        "src/dawn/native/webgpu_absl_format_autogen.cpp",
        "src/dawn/native/wgpu_structs_autogen.cpp",
    ],
)

filegroup(
    name = "gen-dawn-wire-hdrs",
    srcs = [
        "src/dawn/wire/ObjectType_autogen.h",
        "src/dawn/wire/WireCmd_autogen.h",
        "src/dawn/wire/client/ApiObjects_autogen.h",
        "src/dawn/wire/client/ClientBase_autogen.h",
        "src/dawn/wire/client/ClientPrototypes_autogen.inc",
        "src/dawn/wire/server/ServerBase_autogen.h",
        "src/dawn/wire/server/ServerPrototypes_autogen.inc",
        "src/dawn/wire/server/WGPUTraits_autogen.h",
    ],
)

filegroup(
    name = "gen-dawn-wire-srcs",
    srcs = [
        "src/dawn/wire/WireCmd_autogen.cpp",
        "src/dawn/wire/client/ApiProcs_autogen.cpp",
        "src/dawn/wire/client/ClientHandlers_autogen.cpp",
        "src/dawn/wire/server/ServerDoers_autogen.cpp",
        "src/dawn/wire/server/ServerHandlers_autogen.cpp",
    ],
)

filegroup(
    name = "gen-dawn-version-hdrs",
    srcs = [
        "src/dawn/common/Version_autogen.h",
    ],
)

filegroup(
    name = "gen-gpu-info-srcs",
    srcs = [
        "src/dawn/common/GPUInfo_autogen.cpp",
        "src/dawn/common/GPUInfo_autogen.h",
    ],
)

filegroup(
    name = "gen-opengl-loader-srcs",
    srcs = [
        "src/dawn/native/opengl/OpenGLFunctionsBase_autogen.cpp",
        "src/dawn/native/opengl/OpenGLFunctionsBase_autogen.h",
        "src/dawn/native/opengl/opengl_platform_autogen.h",
    ],
)

# ref src/dawn/BUILD.gn
cc_library(
    name = "dawn-proc",
    srcs = [
        "src/dawn/common/Compiler.h",
        "src/dawn/common/Log.h",
        ":gen-dawn-proc-srcs",
    ],
    hdrs = [
        "include/dawn/dawn_proc.h",
        "include/webgpu/webgpu.h",
    ],
    includes = [
        "include",
        "src",
    ],
    linkstatic = 1,
    visibility = ["//visibility:public"],
    deps = [
        ":dawn-hdrs",
        ":gen-dawn-hdrs",
        "//src/utils:utils",
    ],
)

cc_library(
    name = "dawn-partition-alloc",
    hdrs = glob(["src/dawn/partition_alloc/partition_alloc/**/*.h"]),
    visibility = ["//visibility:public"],
)

nia_objc_library(
    name = "dawn-common-sys-utils-objc",
    srcs = [
        "src/dawn/common/Assert.h",
        "src/dawn/common/Compiler.h",
        "src/dawn/common/Platform.h",
        "src/dawn/common/SystemUtils_mac.mm",
    ],
    hdrs = [
        "src/dawn/common/SystemUtils.h",
    ],
    copts = COMMON_COPTS,
    includes = [
        "src",
    ],
    linkopts = select({
        "@the8thwall//bzl/conditions:apple": [
            "-framework CoreFoundation",
        ],
        "//conditions:default": [],
    }),
    deps = [
        "//src/utils:utils",
    ],
)

# ref src/dawn/common/BUILD.gn
cc_library(
    name = "dawn-common",
    srcs = [
        ":dawn-share-common-native",
        ":gen-dawn-cpp-hdrs",
        ":gen-gpu-info-srcs",
        "src/dawn/common/AlignedAlloc.cpp",
        "src/dawn/common/AlignedAlloc.h",
        "src/dawn/common/Alloc.h",
        "src/dawn/common/Assert.cpp",
        "src/dawn/common/Assert.h",
        "src/dawn/common/BitSetIterator.h",
        "src/dawn/common/Compiler.h",
        "src/dawn/common/Constants.h",
        "src/dawn/common/ContentLessObjectCache.h",
        "src/dawn/common/ContentLessObjectCacheable.h",
        "src/dawn/common/CoreFoundationRef.h",
        "src/dawn/common/DynamicLib.cpp",
        "src/dawn/common/DynamicLib.h",
        "src/dawn/common/Enumerator.h",
        "src/dawn/common/FutureUtils.cpp",
        "src/dawn/common/FutureUtils.h",
        "src/dawn/common/GPUInfo.cpp",
        "src/dawn/common/GPUInfo.h",
        "src/dawn/common/HashUtils.h",
        "src/dawn/common/IOKitRef.h",
        "src/dawn/common/LinkedList.h",
        "src/dawn/common/Log.cpp",
        "src/dawn/common/Log.h",
        "src/dawn/common/MatchVariant.h",
        "src/dawn/common/Math.cpp",
        "src/dawn/common/Math.h",
        "src/dawn/common/Mutex.cpp",
        "src/dawn/common/Mutex.h",
        "src/dawn/common/MutexProtected.h",
        "src/dawn/common/NSRef.h",
        "src/dawn/common/NonCopyable.h",
        "src/dawn/common/NonMovable.h",
        "src/dawn/common/Numeric.h",
        "src/dawn/common/PlacementAllocated.h",
        "src/dawn/common/Platform.h",
        "src/dawn/common/Preprocessor.h",
        "src/dawn/common/Range.h",
        "src/dawn/common/Ref.h",
        "src/dawn/common/RefBase.h",
        "src/dawn/common/RefCounted.cpp",
        "src/dawn/common/RefCounted.h",
        "src/dawn/common/Result.cpp",
        "src/dawn/common/Result.h",
        "src/dawn/common/SerialMap.h",
        "src/dawn/common/SerialQueue.h",
        "src/dawn/common/SerialStorage.h",
        "src/dawn/common/SlabAllocator.cpp",
        "src/dawn/common/SlabAllocator.h",
        "src/dawn/common/StackAllocated.h",
        "src/dawn/common/StringViewUtils.cpp",
        "src/dawn/common/StringViewUtils.h",
        "src/dawn/common/SystemUtils.cpp",
        "src/dawn/common/SystemUtils.h",
        "src/dawn/common/TypeTraits.h",
        "src/dawn/common/TypedInteger.h",
        "src/dawn/common/UnderlyingType.h",
        "src/dawn/common/WGSLFeatureMapping.h",
        "src/dawn/common/WeakRef.h",
        "src/dawn/common/WeakRefSupport.cpp",
        "src/dawn/common/WeakRefSupport.h",
        "src/dawn/common/egl_platform.h",
        "src/dawn/common/ityp_array.h",
        "src/dawn/common/ityp_bitset.h",
        "src/dawn/common/ityp_span.h",
        "src/dawn/common/ityp_stack_vec.h",
        "src/dawn/common/ityp_vector.h",
        "src/dawn/common/vulkan_platform.h",
        "src/dawn/common/xlib_with_undefs.h",
    ] + select({
        "@the8thwall//bzl/conditions:apple": [
            "src/dawn/common/IOSurfaceUtils.h",
            "src/dawn/common/IOSurfaceUtils.cpp",
        ],
        "@the8thwall//bzl/conditions:windows": [
            "src/dawn/common/WindowsUtils.cpp",
            "src/dawn/common/WindowsUtils.h",
            "src/dawn/common/windows_with_undefs.h",
        ],
        "//conditions:default": [],
    }),
    hdrs = [
        "include/webgpu/webgpu.h",
        "src/dawn/common/WGSLFeatureMapping.h",
        ":dawn-webgpu-hdrs",
    ],
    copts = COMMON_COPTS,
    includes = [
        "include",
        "src",
        "src/dawn/partition_alloc",
    ],
    linkstatic = 1,
    visibility = ["//visibility:public"],
    deps = [
        ":dawn-hdrs",
        ":dawn-partition-alloc",
        ":gen-dawn-hdrs",
        "//src/utils:utils",
        "@com_google_absl//absl/container:inlined_vector",
    ] + select({
        "@the8thwall//bzl/conditions:apple": [
            ":dawn-common-sys-utils-objc",
        ],
        "//conditions:default": [],
    }),
)

# ref src/dawn/platform/BUILD.gn
cc_library(
    name = "dawn-platform",
    srcs = [
        "src/dawn/platform/DawnPlatform.cpp",
        "src/dawn/platform/WorkerThread.cpp",
        "src/dawn/platform/WorkerThread.h",
        "src/dawn/platform/metrics/HistogramMacros.cpp",
        "src/dawn/platform/metrics/HistogramMacros.h",
        "src/dawn/platform/tracing/EventTracer.cpp",
        "src/dawn/platform/tracing/EventTracer.h",
        "src/dawn/platform/tracing/TraceEvent.h",
    ],
    hdrs = [
        "include/dawn/platform/DawnPlatform.h",
        "include/dawn/platform/dawn_platform_export.h",
    ],
    copts = COMMON_COPTS,
    includes = [
        "include",
        "src",
    ],
    linkstatic = 1,
    visibility = ["//visibility:public"],
    deps = [
        ":dawn-common",
    ],
)

filegroup(
    name = "dawn-share-common-native",
    srcs = [
        "src/dawn/common/RefCountedWithExternalCount.h",
    ],
)

# ref src/dawn/native/BUILD.gn

filegroup(
    name = "dawn-native-backend-hrs",
    srcs = [
        "include/dawn/native/DawnNative.h",
        "include/dawn/native/dawn_native_export.h",
        # Include all backend's public headers so that dependencies can include
        # them even when the backends are disabled.
        "include/dawn/native/D3D11Backend.h",
        "include/dawn/native/D3D12Backend.h",
        "include/dawn/native/D3DBackend.h",
        "include/dawn/native/MetalBackend.h",
        "include/dawn/native/NullBackend.h",
        "include/dawn/native/OpenGLBackend.h",
        "include/dawn/native/VulkanBackend.h",
    ],
)

filegroup(
    name = "dawn-native-header-srcs",
    srcs = [
        "src/dawn/native/Adapter.h",
        "src/dawn/native/ApplyClearColorValueWithDrawHelper.h",
        "src/dawn/native/AsyncTask.h",
        "src/dawn/native/AttachmentState.h",
        "src/dawn/native/BackendConnection.h",
        "src/dawn/native/BindGroup.h",
        "src/dawn/native/BindGroupLayout.h",
        "src/dawn/native/BindGroupLayoutInternal.h",
        "src/dawn/native/BindGroupTracker.h",
        "src/dawn/native/BindingInfo.h",
        "src/dawn/native/BlitBufferToDepthStencil.h",
        "src/dawn/native/BlitColorToColorWithDraw.h",
        "src/dawn/native/BlitDepthToDepth.h",
        "src/dawn/native/BlitTextureToBuffer.h",
        "src/dawn/native/Blob.h",
        "src/dawn/native/BlobCache.h",
        "src/dawn/native/BuddyAllocator.h",
        "src/dawn/native/BuddyMemoryAllocator.h",
        "src/dawn/native/Buffer.h",
        "src/dawn/native/CacheKey.h",
        "src/dawn/native/CacheRequest.h",
        "src/dawn/native/CacheResult.h",
        "src/dawn/native/CachedObject.h",
        "src/dawn/native/CallbackTaskManager.h",
        "src/dawn/native/ChainUtils.h",
        "src/dawn/native/ChainUtilsImpl.inl",
        "src/dawn/native/CommandAllocator.h",
        "src/dawn/native/CommandBuffer.h",
        "src/dawn/native/CommandBufferStateTracker.h",
        "src/dawn/native/CommandEncoder.h",
        "src/dawn/native/CommandValidation.h",
        "src/dawn/native/Commands.h",
        "src/dawn/native/CompilationMessages.h",
        "src/dawn/native/ComputePassEncoder.h",
        "src/dawn/native/ComputePipeline.h",
        "src/dawn/native/CopyTextureForBrowserHelper.h",
        "src/dawn/native/CreatePipelineAsyncEvent.h",
        "src/dawn/native/Device.h",
        "src/dawn/native/DynamicUploader.h",
        "src/dawn/native/EncodingContext.h",
        "src/dawn/native/EnumClassBitmasks.h",
        "src/dawn/native/EnumMaskIterator.h",
        "src/dawn/native/Error.h",
        "src/dawn/native/ErrorData.h",
        "src/dawn/native/ErrorInjector.h",
        "src/dawn/native/ErrorScope.h",
        "src/dawn/native/ErrorSink.h",
        "src/dawn/native/EventManager.h",
        "src/dawn/native/ExecutionQueue.h",
        "src/dawn/native/ExternalTexture.h",
        "src/dawn/native/Features.h",
        "src/dawn/native/Format.h",
        "src/dawn/native/Forward.h",
        "src/dawn/native/IndirectDrawMetadata.h",
        "src/dawn/native/IndirectDrawValidationEncoder.h",
        "src/dawn/native/Instance.h",
        "src/dawn/native/IntegerTypes.h",
        "src/dawn/native/InternalPipelineStore.h",
        "src/dawn/native/Limits.h",
        "src/dawn/native/ObjectBase.h",
        "src/dawn/native/ObjectContentHasher.h",
        "src/dawn/native/PassResourceUsage.h",
        "src/dawn/native/PassResourceUsageTracker.h",
        "src/dawn/native/PerStage.h",
        "src/dawn/native/PhysicalDevice.h",
        "src/dawn/native/Pipeline.h",
        "src/dawn/native/PipelineCache.h",
        "src/dawn/native/PipelineLayout.h",
        "src/dawn/native/PooledResourceMemoryAllocator.h",
        "src/dawn/native/ProgrammableEncoder.h",
        "src/dawn/native/QueryHelper.h",
        "src/dawn/native/QuerySet.h",
        "src/dawn/native/Queue.h",
        "src/dawn/native/RenderBundle.h",
        "src/dawn/native/RenderBundleEncoder.h",
        "src/dawn/native/RenderEncoderBase.h",
        "src/dawn/native/RenderPassEncoder.h",
        "src/dawn/native/RenderPassWorkaroundsHelper.h",
        "src/dawn/native/RenderPipeline.h",
        "src/dawn/native/ResourceHeap.h",
        "src/dawn/native/ResourceHeapAllocator.h",
        "src/dawn/native/ResourceMemoryAllocation.h",
        "src/dawn/native/RingBufferAllocator.h",
        "src/dawn/native/Sampler.h",
        "src/dawn/native/ScratchBuffer.h",
        "src/dawn/native/Serializable.h",
        "src/dawn/native/ShaderModule.h",
        "src/dawn/native/SharedBufferMemory.h",
        "src/dawn/native/SharedFence.h",
        "src/dawn/native/SharedResourceMemory.h",
        "src/dawn/native/SharedTextureMemory.h",
        "src/dawn/native/Subresource.h",
        "src/dawn/native/SubresourceStorage.h",
        "src/dawn/native/Surface.h",
        "src/dawn/native/SwapChain.h",
        "src/dawn/native/SystemEvent.h",
        "src/dawn/native/SystemHandle.h",
        "src/dawn/native/Texture.h",
        "src/dawn/native/TintUtils.h",
        "src/dawn/native/ToBackend.h",
        "src/dawn/native/Toggles.h",
        "src/dawn/native/UsageValidationMode.h",
        "src/dawn/native/VisitableMembers.h",
        "src/dawn/native/WaitAnySystemEvent.h",
        "src/dawn/native/dawn_platform.h",
        "src/dawn/native/stream/BlobSource.h",
        "src/dawn/native/stream/ByteVectorSink.h",
        "src/dawn/native/stream/Sink.h",
        "src/dawn/native/stream/Source.h",
        "src/dawn/native/stream/Stream.h",
        "src/dawn/native/utils/WGPUHelpers.h",
        "src/dawn/native/webgpu_absl_format.h",
        ":dawn-native-hdrs",
        ":dawn-share-common-native",
    ],
)

filegroup(
    name = "dawn-native-cc-srcs",
    srcs = [
        "src/dawn/native/Adapter.cpp",
        "src/dawn/native/ApplyClearColorValueWithDrawHelper.cpp",
        "src/dawn/native/AsyncTask.cpp",
        "src/dawn/native/AttachmentState.cpp",
        "src/dawn/native/BackendConnection.cpp",
        "src/dawn/native/BindGroup.cpp",
        "src/dawn/native/BindGroupLayout.cpp",
        "src/dawn/native/BindGroupLayoutInternal.cpp",
        "src/dawn/native/BindingInfo.cpp",
        "src/dawn/native/BlitBufferToDepthStencil.cpp",
        "src/dawn/native/BlitColorToColorWithDraw.cpp",
        "src/dawn/native/BlitDepthToDepth.cpp",
        "src/dawn/native/BlitTextureToBuffer.cpp",
        "src/dawn/native/Blob.cpp",
        "src/dawn/native/BlobCache.cpp",
        "src/dawn/native/BuddyAllocator.cpp",
        "src/dawn/native/BuddyMemoryAllocator.cpp",
        "src/dawn/native/Buffer.cpp",
        "src/dawn/native/CacheKey.cpp",
        "src/dawn/native/CacheRequest.cpp",
        "src/dawn/native/CachedObject.cpp",
        "src/dawn/native/CallbackTaskManager.cpp",
        "src/dawn/native/CommandAllocator.cpp",
        "src/dawn/native/CommandBuffer.cpp",
        "src/dawn/native/CommandBufferStateTracker.cpp",
        "src/dawn/native/CommandEncoder.cpp",
        "src/dawn/native/CommandValidation.cpp",
        "src/dawn/native/Commands.cpp",
        "src/dawn/native/CompilationMessages.cpp",
        "src/dawn/native/ComputePassEncoder.cpp",
        "src/dawn/native/ComputePipeline.cpp",
        "src/dawn/native/CopyTextureForBrowserHelper.cpp",
        "src/dawn/native/CreatePipelineAsyncEvent.cpp",
        "src/dawn/native/DawnNative.cpp",
        "src/dawn/native/Device.cpp",
        "src/dawn/native/DynamicUploader.cpp",
        "src/dawn/native/EncodingContext.cpp",
        "src/dawn/native/Error.cpp",
        "src/dawn/native/ErrorData.cpp",
        "src/dawn/native/ErrorInjector.cpp",
        "src/dawn/native/ErrorScope.cpp",
        "src/dawn/native/EventManager.cpp",
        "src/dawn/native/ExecutionQueue.cpp",
        "src/dawn/native/ExternalTexture.cpp",
        "src/dawn/native/Features.cpp",
        "src/dawn/native/Format.cpp",
        "src/dawn/native/IndirectDrawMetadata.cpp",
        "src/dawn/native/IndirectDrawValidationEncoder.cpp",
        "src/dawn/native/Instance.cpp",
        "src/dawn/native/InternalPipelineStore.cpp",
        "src/dawn/native/Limits.cpp",
        "src/dawn/native/ObjectBase.cpp",
        "src/dawn/native/ObjectContentHasher.cpp",
        "src/dawn/native/PassResourceUsage.cpp",
        "src/dawn/native/PassResourceUsageTracker.cpp",
        "src/dawn/native/PerStage.cpp",
        "src/dawn/native/PhysicalDevice.cpp",
        "src/dawn/native/Pipeline.cpp",
        "src/dawn/native/PipelineCache.cpp",
        "src/dawn/native/PipelineLayout.cpp",
        "src/dawn/native/PooledResourceMemoryAllocator.cpp",
        "src/dawn/native/ProgrammableEncoder.cpp",
        "src/dawn/native/QueryHelper.cpp",
        "src/dawn/native/QuerySet.cpp",
        "src/dawn/native/Queue.cpp",
        "src/dawn/native/RenderBundle.cpp",
        "src/dawn/native/RenderBundleEncoder.cpp",
        "src/dawn/native/RenderEncoderBase.cpp",
        "src/dawn/native/RenderPassEncoder.cpp",
        "src/dawn/native/RenderPassWorkaroundsHelper.cpp",
        "src/dawn/native/RenderPipeline.cpp",
        "src/dawn/native/ResourceMemoryAllocation.cpp",
        "src/dawn/native/RingBufferAllocator.cpp",
        "src/dawn/native/Sampler.cpp",
        "src/dawn/native/ScratchBuffer.cpp",
        "src/dawn/native/ShaderModule.cpp",
        "src/dawn/native/SharedBufferMemory.cpp",
        "src/dawn/native/SharedFence.cpp",
        "src/dawn/native/SharedResourceMemory.cpp",
        "src/dawn/native/SharedTextureMemory.cpp",
        "src/dawn/native/Subresource.cpp",
        "src/dawn/native/Surface.cpp",
        "src/dawn/native/SwapChain.cpp",
        "src/dawn/native/SystemEvent.cpp",
        "src/dawn/native/SystemHandle.cpp",
        "src/dawn/native/Texture.cpp",
        "src/dawn/native/TintUtils.cpp",
        "src/dawn/native/Toggles.cpp",
        "src/dawn/native/stream/BlobSource.cpp",
        "src/dawn/native/stream/ByteVectorSink.cpp",
        "src/dawn/native/stream/Stream.cpp",
        "src/dawn/native/utils/WGPUHelpers.cpp",
        "src/dawn/native/webgpu_absl_format.cpp",
    ],
)

# dawn_native metal
nia_objc_library(
    name = "dawn-native-backend-metal-objc",
    srcs = [
        "src/dawn/native/Surface_metal.mm",
        "src/dawn/native/metal/BackendMTL.h",
        "src/dawn/native/metal/BackendMTL.mm",
        "src/dawn/native/metal/BindGroupLayoutMTL.h",
        "src/dawn/native/metal/BindGroupLayoutMTL.mm",
        "src/dawn/native/metal/BindGroupMTL.h",
        "src/dawn/native/metal/BindGroupMTL.mm",
        "src/dawn/native/metal/BufferMTL.h",
        "src/dawn/native/metal/BufferMTL.mm",
        "src/dawn/native/metal/CommandBufferMTL.h",
        "src/dawn/native/metal/CommandBufferMTL.mm",
        "src/dawn/native/metal/CommandRecordingContext.h",
        "src/dawn/native/metal/CommandRecordingContext.mm",
        "src/dawn/native/metal/ComputePipelineMTL.h",
        "src/dawn/native/metal/ComputePipelineMTL.mm",
        "src/dawn/native/metal/DeviceMTL.h",
        "src/dawn/native/metal/DeviceMTL.mm",
        "src/dawn/native/metal/Forward.h",
        "src/dawn/native/metal/MetalBackend.mm",
        "src/dawn/native/metal/MultiDrawEncoder.h",
        "src/dawn/native/metal/MultiDrawEncoder.mm",
        "src/dawn/native/metal/PhysicalDeviceMTL.h",
        "src/dawn/native/metal/PhysicalDeviceMTL.mm",
        "src/dawn/native/metal/PipelineLayoutMTL.h",
        "src/dawn/native/metal/PipelineLayoutMTL.mm",
        "src/dawn/native/metal/QuerySetMTL.h",
        "src/dawn/native/metal/QuerySetMTL.mm",
        "src/dawn/native/metal/QueueMTL.h",
        "src/dawn/native/metal/QueueMTL.mm",
        "src/dawn/native/metal/RenderPipelineMTL.h",
        "src/dawn/native/metal/RenderPipelineMTL.mm",
        "src/dawn/native/metal/SamplerMTL.h",
        "src/dawn/native/metal/SamplerMTL.mm",
        "src/dawn/native/metal/ShaderModuleMTL.h",
        "src/dawn/native/metal/ShaderModuleMTL.mm",
        "src/dawn/native/metal/SharedFenceMTL.h",
        "src/dawn/native/metal/SharedFenceMTL.mm",
        "src/dawn/native/metal/SharedTextureMemoryMTL.h",
        "src/dawn/native/metal/SharedTextureMemoryMTL.mm",
        "src/dawn/native/metal/SwapChainMTL.h",
        "src/dawn/native/metal/SwapChainMTL.mm",
        "src/dawn/native/metal/TextureMTL.h",
        "src/dawn/native/metal/TextureMTL.mm",
        "src/dawn/native/metal/UtilsMetal.h",
        "src/dawn/native/metal/UtilsMetal.mm",
        ":gen-dawn-native-utils-hdrs",
        ":gen-dawn-native-utils-srcs",
    ],
    hdrs = [
        "include/tint/tint.h",
        ":dawn-native-backend-hrs",
        ":dawn-native-header-srcs",
        ":dawn-webgpu-hdrs",
        ":gen-dawn-cpp-hdrs",
    ],
    copts = COMMON_COPTS + [
        "-fno-objc-arc",
    ],
    includes = [
        "include",
        "src",
    ],
    linkopts = [
        "-framework IOKit",
        "-framework IOSurface",
        "-framework Metal",
        "-framework QuartzCore",
    ] + select({
        "@the8thwall//bzl/conditions:osx": [
            "-framework Cocoa",
        ],
        "//conditions:default": [],
    }),
    deps = [
        ":dawn-common",
        ":dawn-platform",
        ":gen-dawn-hdrs",
        "//src/tint/api",
        "@com_google_absl//absl/container:flat_hash_map",
        "@com_google_absl//absl/container:flat_hash_set",
    ],
)

cc_library(
    name = "dawn-native-backend-null",
    srcs = [
        "src/dawn/native/null/DeviceNull.cpp",
        "src/dawn/native/null/DeviceNull.h",
        "src/dawn/native/null/NullBackend.cpp",
        ":dawn-native-header-srcs",
        ":gen-dawn-native-utils-hdrs",
    ],
    hdrs = [
        ":dawn-native-backend-hrs",
        "include/dawn/platform/DawnPlatform.h",
        "include/dawn/platform/dawn_platform_export.h",
        "include/tint/tint.h",
    ],
    copts = COMMON_COPTS,
    includes = [
        "include",
        "src",
    ],
    visibility = ["//visibility:public"],
    deps = [
        ":dawn-common",
        "//src/tint/api",
        "@com_google_absl//absl/container:flat_hash_map",
        "@com_google_absl//absl/container:flat_hash_set",
        "@com_google_absl//absl/strings:str_format",
    ],
)

cc_library(
    name = "dawn-native-backend-gles",
    srcs = [
        ":dawn-native-header-srcs",
        ":gen-dawn-native-utils-hdrs",
    ] + select({
        "@the8thwall//bzl/conditions:android": [
            #"src/dawn/native/AHBFunctions.cpp",
            #"src/dawn/native/AHBFunctions.h",
        ],
        "@the8thwall//bzl/conditions:linux": [
        ],
        "//conditions:default": [],
    }),
    hdrs = [
        ":dawn-native-backend-hrs",
        "include/dawn/platform/DawnPlatform.h",
        "include/dawn/platform/dawn_platform_export.h",
        "include/tint/tint.h",
    ],
    copts = COMMON_COPTS,
    includes = [
        "include",
        "src",
    ],
    visibility = ["//visibility:public"],
    deps = [
        ":dawn-common",
        ":dawn-platform",
        "//src/tint/api:api",
        "@com_google_absl//absl/container:flat_hash_map",
        "@com_google_absl//absl/container:flat_hash_set",
        "@com_google_absl//absl/strings:str_format",
        "@vulkan_headers//:vulkan_headers",
    ],
)

cc_library(
    name = "dawn-native-spirv-lib",
    srcs = [
        "src/dawn/native/SpirvValidation.cpp",
        ":dawn-native-header-srcs",
        ":gen-dawn-native-utils-hdrs",
    ],
    hdrs = [
        "include/dawn/native/DawnNative.h",
        "include/dawn/native/dawn_native_export.h",
        "include/tint/tint.h",
        "src/dawn/native/SpirvValidation.h",
    ],
    copts = COMMON_COPTS,
    includes = [
        "include",
        "src",
    ],
    visibility = ["//visibility:public"],
    deps = [
        ":dawn-common",
        "//src/tint/api",
        "@com_google_absl//absl/container:flat_hash_map",
        "@com_google_absl//absl/container:flat_hash_set",
        "@com_google_absl//absl/strings:str_format",
        "@spirv_tools//:spirv_tools_opt",
    ],
)

filegroup(
    name = "dawn-native-vulkan-hdrs",
    srcs = [
        "src/dawn/native/vulkan/BackendVk.h",
        "src/dawn/native/vulkan/BindGroupLayoutVk.h",
        "src/dawn/native/vulkan/BindGroupVk.h",
        "src/dawn/native/vulkan/BufferVk.h",
        "src/dawn/native/vulkan/CommandBufferVk.h",
        "src/dawn/native/vulkan/CommandRecordingContextVk.h",
        "src/dawn/native/vulkan/ComputePipelineVk.h",
        "src/dawn/native/vulkan/DescriptorSetAllocation.h",
        "src/dawn/native/vulkan/DescriptorSetAllocator.h",
        "src/dawn/native/vulkan/DeviceVk.h",
        "src/dawn/native/vulkan/ExternalHandle.h",
        "src/dawn/native/vulkan/FencedDeleter.h",
        "src/dawn/native/vulkan/Forward.h",
        "src/dawn/native/vulkan/PhysicalDeviceVk.h",
        "src/dawn/native/vulkan/PipelineCacheVk.h",
        "src/dawn/native/vulkan/PipelineLayoutVk.h",
        "src/dawn/native/vulkan/PipelineVk.h",
        "src/dawn/native/vulkan/QuerySetVk.h",
        "src/dawn/native/vulkan/QueueVk.h",
        "src/dawn/native/vulkan/RefCountedVkHandle.h",
        "src/dawn/native/vulkan/RenderPassCache.h",
        "src/dawn/native/vulkan/RenderPipelineVk.h",
        "src/dawn/native/vulkan/ResolveTextureLoadingUtilsVk.h",
        "src/dawn/native/vulkan/ResourceHeapVk.h",
        "src/dawn/native/vulkan/ResourceMemoryAllocatorVk.h",
        "src/dawn/native/vulkan/SamplerVk.h",
        "src/dawn/native/vulkan/ShaderModuleVk.h",
        "src/dawn/native/vulkan/SharedFenceVk.h",
        "src/dawn/native/vulkan/SharedTextureMemoryVk.h",
        "src/dawn/native/vulkan/SwapChainVk.h",
        "src/dawn/native/vulkan/TextureVk.h",
        "src/dawn/native/vulkan/UniqueVkHandle.h",
        "src/dawn/native/vulkan/UtilsVulkan.h",
        "src/dawn/native/vulkan/VulkanError.h",
        "src/dawn/native/vulkan/VulkanExtensions.h",
        "src/dawn/native/vulkan/VulkanFunctions.h",
        "src/dawn/native/vulkan/VulkanInfo.h",
        "src/dawn/native/vulkan/external_memory/MemoryImportParams.h",
        "src/dawn/native/vulkan/external_memory/MemoryService.h",
        "src/dawn/native/vulkan/external_memory/MemoryServiceImplementation.h",
        "src/dawn/native/vulkan/external_semaphore/SemaphoreService.h",
        "src/dawn/native/vulkan/external_semaphore/SemaphoreServiceImplementation.h",
    ],
)

filegroup(
    name = "dawn-native-vulkan-srcs",
    srcs = [
        "src/dawn/native/vulkan/VulkanBackend.cpp",
        "src/dawn/native/vulkan/BackendVk.cpp",
        "src/dawn/native/vulkan/BindGroupLayoutVk.cpp",
        "src/dawn/native/vulkan/BindGroupVk.cpp",
        "src/dawn/native/vulkan/BufferVk.cpp",
        "src/dawn/native/vulkan/CommandBufferVk.cpp",
        "src/dawn/native/vulkan/CommandRecordingContextVk.cpp",
        "src/dawn/native/vulkan/ComputePipelineVk.cpp",
        "src/dawn/native/vulkan/DescriptorSetAllocator.cpp",
        "src/dawn/native/vulkan/DeviceVk.cpp",
        "src/dawn/native/vulkan/FencedDeleter.cpp",
        #"src/dawn/native/vulkan/PhysicalDeviceVk.cpp",  # TODO(yuyan): enable after NDK v26
        "src/dawn/native/vulkan/PipelineCacheVk.cpp",
        "src/dawn/native/vulkan/PipelineLayoutVk.cpp",
        "src/dawn/native/vulkan/PipelineVk.cpp",
        "src/dawn/native/vulkan/QuerySetVk.cpp",
        "src/dawn/native/vulkan/QueueVk.cpp",
        "src/dawn/native/vulkan/RenderPassCache.cpp",
        "src/dawn/native/vulkan/RenderPipelineVk.cpp",
        "src/dawn/native/vulkan/ResolveTextureLoadingUtilsVk.cpp",
        "src/dawn/native/vulkan/ResourceHeapVk.cpp",
        "src/dawn/native/vulkan/ResourceMemoryAllocatorVk.cpp",
        "src/dawn/native/vulkan/SamplerVk.cpp",
        "src/dawn/native/vulkan/ShaderModuleVk.cpp",
        "src/dawn/native/vulkan/SharedFenceVk.cpp",
        #"src/dawn/native/vulkan/SharedTextureMemoryVk.cpp",  # TODO(yuyan): enable after NDK v26
        "src/dawn/native/vulkan/StreamImplVk.cpp",
        "src/dawn/native/vulkan/SwapChainVk.cpp",
        "src/dawn/native/vulkan/TextureVk.cpp",
        "src/dawn/native/vulkan/UtilsVulkan.cpp",
        "src/dawn/native/vulkan/VulkanError.cpp",
        "src/dawn/native/vulkan/VulkanExtensions.cpp",
        "src/dawn/native/vulkan/VulkanFunctions.cpp",
        "src/dawn/native/vulkan/VulkanInfo.cpp",
        "src/dawn/native/vulkan/external_memory/MemoryService.cpp",
        "src/dawn/native/vulkan/external_memory/MemoryServiceImplementation.cpp",
        "src/dawn/native/vulkan/external_semaphore/SemaphoreService.cpp",
        "src/dawn/native/vulkan/external_semaphore/SemaphoreServiceImplementation.cpp",
    ],
)

cc_library(
    name = "dawn-native-backend-vulkan",
    srcs = [
        ":dawn-native-header-srcs",
        ":gen-dawn-native-utils-hdrs",
        ":gen-dawn-version-hdrs",
        ":dawn-native-vulkan-hdrs",
        ":dawn-native-vulkan-srcs",
    ] + select({
        "@the8thwall//bzl/conditions:android": [
            #"src/dawn/native/AHBFunctions.cpp",  # TODO(yuyan): enable after NDK v26
            #"src/dawn/native/AHBFunctions.h",  # TODO(yuyan): enable after NDK v26
            "src/dawn/native/vulkan/external_memory/MemoryServiceImplementationAHardwareBuffer.cpp",
            "src/dawn/native/vulkan/external_memory/MemoryServiceImplementationAHardwareBuffer.h",
            "src/dawn/native/vulkan/external_semaphore/SemaphoreServiceImplementationFD.cpp",
            "src/dawn/native/vulkan/external_semaphore/SemaphoreServiceImplementationFD.h",
        ],
        "@the8thwall//bzl/conditions:linux": [
            "src/dawn/native/vulkan/external_memory/MemoryServiceImplementationDmaBuf.cpp",
            "src/dawn/native/vulkan/external_memory/MemoryServiceImplementationDmaBuf.h",
            "src/dawn/native/vulkan/external_memory/MemoryServiceImplementationOpaqueFD.cpp",
            "src/dawn/native/vulkan/external_memory/MemoryServiceImplementationOpaqueFD.h",
            "src/dawn/native/vulkan/external_semaphore/SemaphoreServiceImplementationFD.cpp",
            "src/dawn/native/vulkan/external_semaphore/SemaphoreServiceImplementationFD.h",
        ],
        "//conditions:default": [],
    }),
    hdrs = [
        ":dawn-native-backend-hrs",
        "include/dawn/platform/DawnPlatform.h",
        "include/dawn/platform/dawn_platform_export.h",
        "include/tint/tint.h",
    ],
    copts = COMMON_COPTS,
    includes = [
        "include",
        "src",
    ],
    visibility = ["//visibility:public"],
    deps = [
        ":dawn-common",
        ":dawn-platform",
        "//src/tint/api",
        "@com_google_absl//absl/container:flat_hash_map",
        "@com_google_absl//absl/container:flat_hash_set",
        "@com_google_absl//absl/strings:str_format",
        "@vulkan-utility-libraries//:vulkan_utility_headers",
        "@vulkan_headers//:vulkan_headers",
    ],
)

# ref src/dawn/native/BUILD.gn
cc_library(
    name = "dawn-native",
    srcs = [
        ":dawn-native-cc-srcs",
        ":dawn-native-header-srcs",
        ":gen-dawn-native-utils-hdrs",
        ":gen-dawn-native-utils-srcs",
        ":gen-dawn-version-hdrs",
    ],
    hdrs = [
        "include/dawn/platform/DawnPlatform.h",
        "include/dawn/platform/dawn_platform_export.h",
    ],
    copts = COMMON_COPTS,
    includes = [
        "include",
        "src",
    ],
    visibility = ["//visibility:public"],
    deps = [
        ":dawn-native-backend-null",
        "//src/tint/api:api",
    ] + select({
        "@the8thwall//bzl/conditions:apple": [
            ":dawn-native-backend-metal-objc",
        ],
        ":android-and-gles": [
            ":dawn-native-backend-gles",
        ],
        ":android-and-vulkan": [
            ":dawn-native-backend-vulkan",
        ],
        ":linux-and-vulkan": [
            ":dawn-native-backend-vulkan",
        ],
        "//conditions:default": [],
    }),
)

# dawn-utils
nia_objc_library(
    name = "dawn-utils-metal-objc",
    srcs = [
        "src/dawn/utils/OSXTimer.cpp",
        "src/dawn/utils/ObjCUtils.h",
        "src/dawn/utils/ObjCUtils.mm",
        "src/dawn/utils/Timer.h",
        ":gen-dawn-cpp-hdrs",
    ],
    hdrs = [
        ":dawn-webgpu-hdrs",
    ],
    copts = COMMON_COPTS + [
        "-fno-objc-arc",
    ],
    includes = [
        "include",
        "src",
    ],
    linkopts = select({
        "@the8thwall//bzl/conditions:apple": [
            "-framework QuartzCore",
        ],
        "//conditions:default": [],
    }),
    deps = [
        ":dawn-common",
        ":dawn-platform",
        ":gen-dawn-hdrs",
        "//src/tint/api",
        "@com_google_absl//absl/container:flat_hash_map",
        "@com_google_absl//absl/container:flat_hash_set",
    ],
)

# ref src/dawn/utils/BUILD.gn
cc_library(
    name = "dawn-utils",
    srcs = [
        ":dawn-native-header-srcs",
        "src/dawn/utils/BinarySemaphore.cpp",
        "src/dawn/utils/BinarySemaphore.h",
        "src/dawn/utils/ComboRenderBundleEncoderDescriptor.cpp",
        "src/dawn/utils/ComboRenderBundleEncoderDescriptor.h",
        "src/dawn/utils/ComboRenderPipelineDescriptor.cpp",
        "src/dawn/utils/ComboRenderPipelineDescriptor.h",
        "src/dawn/utils/CommandLineParser.cpp",
        "src/dawn/utils/CommandLineParser.h",
        "src/dawn/utils/EmptyDebugLogger.cpp",
        "src/dawn/utils/PlatformDebugLogger.h",
        "src/dawn/utils/SystemUtils.cpp",
        "src/dawn/utils/SystemUtils.h",
        "src/dawn/utils/TerribleCommandBuffer.cpp",
        "src/dawn/utils/TerribleCommandBuffer.h",
        "src/dawn/utils/TestUtils.cpp",
        "src/dawn/utils/TestUtils.h",
        "src/dawn/utils/TextureUtils.cpp",
        "src/dawn/utils/TextureUtils.h",
        "src/dawn/utils/Timer.h",
        "src/dawn/utils/WGPUHelpers.cpp",
        "src/dawn/utils/WGPUHelpers.h",
        "src/dawn/utils/WireHelper.cpp",
        "src/dawn/utils/WireHelper.h",
    ] + select({
        "@the8thwall//bzl/conditions:apple": [
        ],
        "@the8thwall//bzl/conditions:windows": [
            "src/dawn/utils/WindowsDebugLogger.cpp",
            "src/dawn/utils/WindowsTimer.cpp",
        ],
        "@the8thwall//bzl/conditions:linux": [
            "src/dawn/utils/PosixTimer.cpp",
        ],
        "//conditions:default": [],
    }),
    hdrs = [
        ":dawn-native-backend-hrs",
        "include/dawn/wire/Wire.h",
        "include/dawn/wire/WireClient.h",
        "include/dawn/wire/WireServer.h",
        "include/dawn/wire/dawn_wire_export.h",
    ],
    copts = COMMON_COPTS,
    includes = [
        "include",
        "src",
    ],
    visibility = ["//visibility:public"],
    deps = [
        ":dawn-common",
        "@com_google_absl//absl/container:flat_hash_map",
        "@com_google_absl//absl/strings:string_view",
        "@spirv_tools//:spirv_tools_opt",
    ] + select({
        "@the8thwall//bzl/conditions:apple": [
            ":dawn-utils-metal-objc",
        ],
        "//conditions:default": [],
    }),
)

# ref src/dawn/wire/BUILD.gn
cc_library(
    name = "dawn-wire",
    srcs = [
        "src/dawn/wire/BufferConsumer.h",
        "src/dawn/wire/BufferConsumer_impl.h",
        "src/dawn/wire/ChunkedCommandHandler.cpp",
        "src/dawn/wire/ChunkedCommandHandler.h",
        "src/dawn/wire/ChunkedCommandSerializer.cpp",
        "src/dawn/wire/ChunkedCommandSerializer.h",
        "src/dawn/wire/ObjectHandle.cpp",
        "src/dawn/wire/ObjectHandle.h",
        "src/dawn/wire/SupportedFeatures.cpp",
        "src/dawn/wire/SupportedFeatures.h",
        "src/dawn/wire/Wire.cpp",
        "src/dawn/wire/WireClient.cpp",
        "src/dawn/wire/WireDeserializeAllocator.cpp",
        "src/dawn/wire/WireDeserializeAllocator.h",
        "src/dawn/wire/WireResult.h",
        "src/dawn/wire/WireServer.cpp",
        "src/dawn/wire/client/Adapter.cpp",
        "src/dawn/wire/client/Adapter.h",
        "src/dawn/wire/client/ApiObjects.h",
        "src/dawn/wire/client/Buffer.cpp",
        "src/dawn/wire/client/Buffer.h",
        "src/dawn/wire/client/Client.cpp",
        "src/dawn/wire/client/Client.h",
        "src/dawn/wire/client/ClientDoers.cpp",
        "src/dawn/wire/client/ClientInlineMemoryTransferService.cpp",
        "src/dawn/wire/client/Device.cpp",
        "src/dawn/wire/client/Device.h",
        "src/dawn/wire/client/EventManager.cpp",
        "src/dawn/wire/client/EventManager.h",
        "src/dawn/wire/client/Instance.cpp",
        "src/dawn/wire/client/Instance.h",
        "src/dawn/wire/client/LimitsAndFeatures.cpp",
        "src/dawn/wire/client/LimitsAndFeatures.h",
        "src/dawn/wire/client/ObjectBase.cpp",
        "src/dawn/wire/client/ObjectBase.h",
        "src/dawn/wire/client/ObjectStore.cpp",
        "src/dawn/wire/client/ObjectStore.h",
        "src/dawn/wire/client/QuerySet.cpp",
        "src/dawn/wire/client/QuerySet.h",
        "src/dawn/wire/client/Queue.cpp",
        "src/dawn/wire/client/Queue.h",
        "src/dawn/wire/client/ShaderModule.cpp",
        "src/dawn/wire/client/ShaderModule.h",
        "src/dawn/wire/client/Surface.cpp",
        "src/dawn/wire/client/Surface.h",
        "src/dawn/wire/client/Texture.cpp",
        "src/dawn/wire/client/Texture.h",
        "src/dawn/wire/server/ObjectStorage.h",
        "src/dawn/wire/server/Server.cpp",
        "src/dawn/wire/server/Server.h",
        "src/dawn/wire/server/ServerAdapter.cpp",
        "src/dawn/wire/server/ServerBuffer.cpp",
        "src/dawn/wire/server/ServerDevice.cpp",
        "src/dawn/wire/server/ServerInlineMemoryTransferService.cpp",
        "src/dawn/wire/server/ServerInstance.cpp",
        "src/dawn/wire/server/ServerQueue.cpp",
        "src/dawn/wire/server/ServerShaderModule.cpp",
        ":gen-dawn-wire-hdrs",
        ":gen-dawn-wire-srcs",
    ],
    hdrs = [
        "include/dawn/wire/Wire.h",
        "include/dawn/wire/WireClient.h",
        "include/dawn/wire/WireServer.h",
        "include/dawn/wire/dawn_wire_export.h",
    ],
    copts = COMMON_COPTS,
    includes = [
        "include",
        "src",
    ],
    linkstatic = 1,
    visibility = ["//visibility:public"],
    deps = [
        ":dawn-common",
        "//src/tint/lang/wgsl/features",
        "@com_google_absl//absl/container:flat_hash_map",
        "@com_google_absl//absl/container:flat_hash_set",
    ],
)

# src/dawn/node/interop/CMakeLists.txt
cc_library(
    name = "dawn-node-interop",
    srcs = [
        "include/dawn/webgpu.h",
        "include/webgpu/webgpu.h",
        "include/webgpu/webgpu_cpp.h",
        "include/webgpu/webgpu_enum_class_bitmasks.h",
        "src/dawn/node/interop/Core.cpp",
        "src/dawn/node/interop/WebGPU.cpp",
        "src/dawn/node/utils/Debug.h",
        ":gen-dawn-cpp-hdrs",
    ],
    hdrs = [
        "src/dawn/node/interop/Core.h",
        "src/dawn/node/interop/NodeAPI.h",
        "src/dawn/node/interop/WebGPU.h",
    ],
    copts = COMMON_COPTS,
    includes = [
        "include",
        "src",
    ],
    linkstatic = 1,
    visibility = ["//visibility:public"],
    deps = [
        "@node-addon-api",
    ],
)

# src/dawn/node/binding/CMakeLists.txt
cc_library(
    name = "dawn-node-binding",
    srcs = [
        "src/dawn/node/binding/AsyncRunner.cpp",
        "src/dawn/node/binding/AsyncRunner.h",
        "src/dawn/node/binding/Converter.cpp",
        "src/dawn/node/binding/Converter.h",
        "src/dawn/node/binding/Errors.cpp",
        "src/dawn/node/binding/Errors.h",
        "src/dawn/node/binding/Flags.cpp",
        "src/dawn/node/binding/Flags.h",
        "src/dawn/node/binding/GPU.cpp",
        "src/dawn/node/binding/GPU.h",
        "src/dawn/node/binding/GPUAdapter.cpp",
        "src/dawn/node/binding/GPUAdapter.h",
        "src/dawn/node/binding/GPUAdapterInfo.cpp",
        "src/dawn/node/binding/GPUAdapterInfo.h",
        "src/dawn/node/binding/GPUBindGroup.cpp",
        "src/dawn/node/binding/GPUBindGroup.h",
        "src/dawn/node/binding/GPUBindGroupLayout.cpp",
        "src/dawn/node/binding/GPUBindGroupLayout.h",
        "src/dawn/node/binding/GPUBuffer.cpp",
        "src/dawn/node/binding/GPUBuffer.h",
        "src/dawn/node/binding/GPUCommandBuffer.cpp",
        "src/dawn/node/binding/GPUCommandBuffer.h",
        "src/dawn/node/binding/GPUCommandEncoder.cpp",
        "src/dawn/node/binding/GPUCommandEncoder.h",
        "src/dawn/node/binding/GPUComputePassEncoder.cpp",
        "src/dawn/node/binding/GPUComputePassEncoder.h",
        "src/dawn/node/binding/GPUComputePipeline.cpp",
        "src/dawn/node/binding/GPUComputePipeline.h",
        "src/dawn/node/binding/GPUDevice.cpp",
        "src/dawn/node/binding/GPUDevice.h",
        "src/dawn/node/binding/GPUPipelineLayout.cpp",
        "src/dawn/node/binding/GPUPipelineLayout.h",
        "src/dawn/node/binding/GPUQuerySet.cpp",
        "src/dawn/node/binding/GPUQuerySet.h",
        "src/dawn/node/binding/GPUQueue.cpp",
        "src/dawn/node/binding/GPUQueue.h",
        "src/dawn/node/binding/GPURenderBundle.cpp",
        "src/dawn/node/binding/GPURenderBundle.h",
        "src/dawn/node/binding/GPURenderBundleEncoder.cpp",
        "src/dawn/node/binding/GPURenderBundleEncoder.h",
        "src/dawn/node/binding/GPURenderPassEncoder.cpp",
        "src/dawn/node/binding/GPURenderPassEncoder.h",
        "src/dawn/node/binding/GPURenderPipeline.cpp",
        "src/dawn/node/binding/GPURenderPipeline.h",
        "src/dawn/node/binding/GPUSampler.cpp",
        "src/dawn/node/binding/GPUSampler.h",
        "src/dawn/node/binding/GPUShaderModule.cpp",
        "src/dawn/node/binding/GPUShaderModule.h",
        "src/dawn/node/binding/GPUSupportedFeatures.cpp",
        "src/dawn/node/binding/GPUSupportedFeatures.h",
        "src/dawn/node/binding/GPUSupportedLimits.cpp",
        "src/dawn/node/binding/GPUSupportedLimits.h",
        "src/dawn/node/binding/GPUTexture.cpp",
        "src/dawn/node/binding/GPUTexture.h",
        "src/dawn/node/binding/GPUTextureView.cpp",
        "src/dawn/node/binding/GPUTextureView.h",
        "src/dawn/node/binding/Split.cpp",
        "src/dawn/node/binding/Split.h",
        "src/dawn/node/binding/TogglesLoader.cpp",
        "src/dawn/node/binding/TogglesLoader.h",
    ],
    hdrs = [
        ":dawn-native-backend-hrs",
        "include/webgpu/webgpu.h",
        "include/webgpu/webgpu_cpp.h",
    ],
    copts = COMMON_COPTS,
    includes = [
        "include",
        "src",
    ],
    linkstatic = 1,
    visibility = ["//visibility:public"],
    deps = [
        ":dawn-node-interop",
        ":gen-dawn-hdrs",
    ],
)

# src/dawn/node/CMakeLists.txt
node_addon(
    name = "dawn-node",
    srcs = [
        "src/dawn/node/Module.cpp",
        "src/dawn/node/NapiSymbols.cpp",
        "src/dawn/node/NapiSymbols.h",
    ],
    copts = COMMON_COPTS,
    linkshared = 1,
    linkstatic = 1,
    deps = [
        ":dawn-common",
        ":dawn-native",
        ":dawn-node-binding",
        ":dawn-node-interop",
        ":dawn-partition-alloc",
        ":dawn-platform",
        ":dawn-proc",
        "//src/tint/api",
        "//src/tint/api/common",
        "//src/tint/lang/core",
        "//src/tint/lang/core/common",
        "//src/tint/lang/core/constant",
        "//src/tint/lang/core/intrinsic",
        "//src/tint/lang/core/ir",
        "//src/tint/lang/core/ir/transform",
        "//src/tint/lang/core/type",
        "//src/tint/lang/hlsl/writer/common",
        "//src/tint/lang/msl",
        "//src/tint/lang/msl/intrinsic",
        "//src/tint/lang/msl/ir",
        "//src/tint/lang/msl/type",
        "//src/tint/lang/msl/writer",
        "//src/tint/lang/msl/writer/ast_printer",
        "//src/tint/lang/msl/writer/ast_raise",
        "//src/tint/lang/msl/writer/common",
        "//src/tint/lang/msl/writer/printer",
        "//src/tint/lang/msl/writer/raise",
        "//src/tint/lang/wgsl",
        "//src/tint/lang/wgsl/ast",
        "//src/tint/lang/wgsl/ast/transform",
        "//src/tint/lang/wgsl/common",
        "//src/tint/lang/wgsl/features",
        "//src/tint/lang/wgsl/helpers",
        "//src/tint/lang/wgsl/inspector",
        "//src/tint/lang/wgsl/intrinsic",
        "//src/tint/lang/wgsl/ir",
        "//src/tint/lang/wgsl/program",
        "//src/tint/lang/wgsl/reader",
        "//src/tint/lang/wgsl/reader/lower",
        "//src/tint/lang/wgsl/reader/parser",
        "//src/tint/lang/wgsl/reader/program_to_ir",
        "//src/tint/lang/wgsl/resolver",
        "//src/tint/lang/wgsl/sem",
        "//src/tint/lang/wgsl/writer",
        "//src/tint/lang/wgsl/writer/ast_printer",
        "//src/tint/lang/wgsl/writer/ir_to_program",
        "//src/tint/lang/wgsl/writer/raise",
        "//src/tint/lang/wgsl/writer/syntax_tree_printer",
        "//src/tint/utils/containers",
        "//src/tint/utils/diagnostic",
        "//src/tint/utils/ice",
        "//src/tint/utils/macros",
        "//src/tint/utils/math",
        "//src/tint/utils/memory",
        "//src/tint/utils/result",
        "//src/tint/utils/rtti",
        "//src/tint/utils/strconv",
        "//src/tint/utils/symbol",
        "//src/tint/utils/system",
        "//src/tint/utils/text",
        "@com_google_absl//absl/base",
        "@com_google_absl//absl/base:log_severity",
        "@com_google_absl//absl/base:malloc_internal",
        "@com_google_absl//absl/container:hashtablez_sampler",
        "@com_google_absl//absl/container:raw_hash_set",
        "@com_google_absl//absl/crc:crc32c",
        "@com_google_absl//absl/debugging:stacktrace",
        "@com_google_absl//absl/debugging:symbolize",
        "@com_google_absl//absl/hash",
        "@com_google_absl//absl/hash:city",
        "@com_google_absl//absl/numeric:int128",
        "@com_google_absl//absl/strings",
        "@com_google_absl//absl/strings:cord",
        "@com_google_absl//absl/strings:string_view",
        "@com_google_absl//absl/synchronization",
        "@com_google_absl//absl/time",
        "@com_google_absl//absl/time/internal/cctz:civil_time",
        "@com_google_absl//absl/time/internal/cctz:time_zone",
        "@com_google_absl//absl/types:bad_optional_access",
        "@com_google_absl//absl/types:bad_variant_access",
        "@the8thwall//bzl/node:node-addons",
    ],
)

filegroup(
    name = "dawn-node-index",
    srcs = ["src/dawn/node/index.js"],
)

filegroup(
    name = "dawn-node-cts",
    srcs = ["src/dawn/node/cts.js"],
)

genrule(
    name = "dawn-addon",
    srcs = [
        ":dawn-node",
        ":dawn-node-index",
        ":dawn-node-cts",
    ],
    outs = [
        "dawn.node",
        "index.js",
        "cts.js",
    ],
    cmd = """
        INPATHS=`echo $$(dirname $(locations :dawn-node)) | cut -d' ' -f1`
        cp $$INPATHS/dawn-node.node $(@D)/dawn.node
        cp $(location :dawn-node-index) $(@D)
        cp $(location :dawn-node-cts) $(@D)
    """,
)

# CTS
# go run ./tools/src/cmd/run-cts/main.go -cts=<local repo/webgpu-cts> \
#   -bin <dawn-addon location> \
#   -output <output file> \
#   -verbose 'webgpu:*'
# https://github.com/gpuweb/cts.git

# ref src/dawn/glfw/BUILD.gn
nia_objc_library(
    name = "dawn-glfw-objc",
    srcs = [
        "src/dawn/glfw/utils_metal.mm",
        ":gen-dawn-cpp-hdrs",
    ],
    hdrs = [
        "include/webgpu/webgpu_glfw.h",
        ":dawn-webgpu-hdrs",
    ],
    copts = COMMON_COPTS + [
        "-fno-objc-arc",
    ],
    includes = [
        "include",
    ],
    linkopts = select({
        "@the8thwall//bzl/conditions:apple": [
            "-framework Metal",
        ],
        "//conditions:default": [],
    }),
    deps = [
        ":gen-dawn-hdrs",
        "@glfw",
    ],
)

cc_library(
    name = "dawn-glfw",
    srcs = [
        "src/dawn/glfw/utils.cpp",
        ":gen-dawn-cpp-hdrs",
    ],
    hdrs = [
        "include/webgpu/webgpu_glfw.h",
        ":dawn-webgpu-hdrs",
    ],
    copts = COMMON_COPTS,
    includes = [
        "include",
    ],
    linkstatic = 1,
    visibility = ["//visibility:public"],
    deps = [
        ":dawn-common",
        "@glfw",
    ] + select({
        "@the8thwall//bzl/conditions:apple": [
            ":dawn-glfw-objc",
        ],
        "//conditions:default": [],
    }),
)

# ref src/dawn/samples/BUILD.gn
cc_library(
    name = "dawn-sample-utils",
    srcs = [
        "src/dawn/samples/SampleUtils.cpp",
        ":gen-dawn-cpp-hdrs",
    ],
    hdrs = [
        "include/webgpu/webgpu.h",
        "src/dawn/samples/SampleUtils.h",
        ":dawn-webgpu-hdrs",
    ],
    copts = COMMON_COPTS,
    includes = [
        "include",
        "src",
    ],
    linkstatic = 1,
    visibility = ["//visibility:public"],
    deps = [
        ":dawn-common",
        ":dawn-glfw",
        ":dawn-native",
        ":dawn-proc",
        ":dawn-utils",
        ":dawn-wire",
    ],
)

cc_binary(
    name = "dawn-samples-hello-triangle",
    srcs = [
        "src/dawn/samples/HelloTriangle.cpp",
    ],
    linkstatic = 1,
    deps = [
        ":dawn-hdrs",
        ":dawn-sample-utils",
    ],
)

cc_binary(
    name = "dawn-samples-compute-boids",
    srcs = [
        "src/dawn/samples/ComputeBoids.cpp",
    ],
    linkstatic = 1,
    deps = [
        ":dawn-hdrs",
        ":dawn-sample-utils",
    ],
)

cc_binary(
    name = "dawn-samples-animometer",
    srcs = [
        "src/dawn/samples/Animometer.cpp",
    ],
    linkstatic = 1,
    deps = [
        ":dawn-hdrs",
        ":dawn-sample-utils",
    ],
)

cc_binary(
    name = "dawn-samples-dawn-info",
    srcs = [
        "src/dawn/samples/DawnInfo.cpp",
    ],
    linkstatic = 1,
    deps = [
        ":dawn-hdrs",
        ":dawn-sample-utils",
    ],
)

cc_binary(
    name = "dawn-samples-manual-surface-test",
    srcs = [
        "src/dawn/samples/ManualSurfaceTest.cpp",
    ],
    linkstatic = 1,
    deps = [
        ":dawn-hdrs",
        ":dawn-sample-utils",
    ],
)
