load("@rules_cc//cc:defs.bzl", "cc_library")

package(default_visibility = ["//darparu:__subpackages__"])

opengl_linkopts = select({
    "@platforms//os:windows": ["-DEFAULTLIB:opengl32.lib"],
    "@platforms//os:linux": [
        "-lGL",
        "-lX11",
    ],
    "@platforms//os:osx": [],
})

cc_library(
    name = "ball",
    srcs = ["ball.cc"],
    hdrs = ["ball.h"],
    data = ["//darparu/renderer/shaders:simple"],
    linkopts = opengl_linkopts,
    deps = [
        "//darparu/renderer:gl_error_macro",
        "//darparu/renderer:renderable",
        "//darparu/renderer:shader",
        "//darparu/renderer:shader_context_manager",
        "@glew//:glew_static",
        "@glfw",
    ],
)

cc_library(
    name = "container",
    srcs = ["container.cc"],
    hdrs = ["container.h"],
    data = ["//darparu/renderer/shaders:simple"],
    linkopts = opengl_linkopts,
    deps = [
        "//darparu/renderer:gl_error_macro",
        "//darparu/renderer:renderable",
        "//darparu/renderer:shader",
        "//darparu/renderer:shader_context_manager",
        "@glew//:glew_static",
        "@glfw",
    ],
)

cc_library(
    name = "light",
    srcs = ["light.cc"],
    hdrs = ["light.h"],
    data = ["//darparu/renderer/shaders:light_cube"],
    linkopts = opengl_linkopts,
    deps = [
        "//darparu/renderer:gl_error_macro",
        "//darparu/renderer:renderable",
        "//darparu/renderer:shader",
        "//darparu/renderer:shader_context_manager",
        "@glew//:glew_static",
        "@glfw",
    ],
)

cc_library(
    name = "mesh_2d",
    srcs = ["mesh_2d.cc"],
    hdrs = ["mesh_2d.h"],
    data = ["//darparu/renderer/shaders:simple_2d"],
    linkopts = opengl_linkopts,
    deps = [
        "//darparu/renderer:gl_error_macro",
        "//darparu/renderer:renderable",
        "//darparu/renderer:shader",
        "//darparu/renderer:shader_context_manager",
        "@glew//:glew_static",
        "@glfw",
    ],
)

cc_library(
    name = "plane",
    srcs = ["plane.cc"],
    hdrs = ["plane.h"],
    data = ["//darparu/renderer/shaders:simple_with_normal_matrix"],
    linkopts = opengl_linkopts,
    deps = [
        "//darparu/renderer:gl_error_macro",
        "//darparu/renderer:renderable",
        "//darparu/renderer:shader",
        "//darparu/renderer:shader_context_manager",
        "@glew//:glew_static",
        "@glfw",
    ],
)

cc_library(
    name = "water",
    srcs = ["water.cc"],
    hdrs = ["water.h"],
    data = ["//darparu/renderer/shaders:basic_lighting"],
    linkopts = opengl_linkopts,
    deps = [
        ":water_normals",
        "//darparu/renderer:algebra",
        "//darparu/renderer:gl_error_macro",
        "//darparu/renderer:renderable",
        "//darparu/renderer:shader",
        "//darparu/renderer:shader_context_manager",
        "//darparu/renderer:texture",
        "@glew//:glew_static",
        "@glfw",
    ],
)

cc_library(
    name = "water_normals",
    srcs = [
        "water_normals.cc",
    ],
    hdrs = ["water_normals.h"],
    deps = [
        "//darparu/renderer:algebra",
    ],
)
