load("@rules_cc//cc:defs.bzl", "cc_library")

package(default_visibility = ["//darparu:__subpackages__"])

opengl_linkopts = select({
    "@platforms//os:windows": ["-DEFAULTLIB:opengl32.lib"],
    "@platforms//os:linux": [
        "-lGL",
        "-lX11",
    ],
    "@platforms//os:osx": [
    ],
})

cc_library(
    name = "renderable",
    hdrs = ["renderable.h"],
)

cc_library(
    name = "renderer",
    srcs = ["renderer.cc"],
    hdrs = ["renderer.h"],
    linkopts = opengl_linkopts,
    deps = [
        ":algebra",
        ":gl_error_macro",
        ":projection_context",
        ":renderable",
        ":shader",
        "//darparu/renderer:camera_texture",
        "@glew//:glew_static",
        "@glfw",
    ],
)

cc_library(
    name = "camera_texture",
    srcs = ["camera_texture.cc"],
    hdrs = ["camera_texture.h"],
    linkopts = opengl_linkopts,
    deps = [
        ":gl_error_macro",
        ":texture",
        "@glew//:glew_static",
        "@glfw",
    ],
)

cc_library(
    name = "algebra",
    srcs = ["algebra.cc"],
    hdrs = ["algebra.h"],
)

cc_library(
    name = "shader",
    srcs = ["shader.cc"],
    hdrs = ["shader.h"],
    linkopts = opengl_linkopts,
    deps = [
        ":gl_error_macro",
        "@glew//:glew_static",
        "@glfw",
    ],
)

cc_library(
    name = "shader_context_manager",
    srcs = ["shader_context_manager.cc"],
    hdrs = ["shader_context_manager.h"],
    linkopts = opengl_linkopts,
    deps = [
        ":gl_error_macro",
        ":shader",
        "@glew//:glew_static",
        "@glfw",
    ],
)

cc_library(
    name = "texture",
    srcs = ["texture.cc"],
    hdrs = ["texture.h"],
    linkopts = opengl_linkopts,
    deps = [
        ":gl_error_macro",
        "@glew//:glew_static",
        "@glfw",
    ],
)

cc_library(
    name = "gl_error_macro",
    srcs = ["gl_error_macro.cc"],
    hdrs = ["gl_error_macro.h"],
    linkopts = opengl_linkopts,
    deps = [
        "@glew//:glew_static",
        "@glfw",
    ],
)

cc_library(
    name = "projection_context",
    hdrs = ["projection_context.h"],
)
