load("@rules_cc//cc:defs.bzl", "cc_binary")

cc_binary(
    name = "darparu_birds_eye",
    srcs = ["darparu_birds_eye.cc"],
    deps = [
        "//darparu/renderer",
        "//darparu/renderer:algebra",
        "//darparu/renderer:projection_context",
        "//darparu/renderer/cameras:pan",
        "//darparu/renderer/entities:container",
        "//darparu/renderer/entities:light",
        "//darparu/renderer/entities:plane",
        "//darparu/renderer/entities:water",
        "//darparu/renderer/io_controls:simple_2d",
    ],
)

cc_binary(
    name = "darparu",
    srcs = ["darparu.cc"],
    deps = [
        "//darparu/renderer",
        "//darparu/renderer/cameras:orbit",
        "//darparu/renderer/entities:ball",
        "//darparu/renderer/entities:container",
        "//darparu/renderer/entities:light",
        "//darparu/renderer/entities:water",
        "//darparu/renderer/io_controls:simple_3d",
    ],
)

cc_binary(
    name = "voronoi",
    srcs = ["voronoi.cc"],
    deps = [
        "//darparu:triangulate_2d",
        "//darparu/renderer",
        "//darparu/renderer:algebra",
        "//darparu/renderer:projection_context",
        "//darparu/renderer/cameras:pan",
        "//darparu/renderer/entities:mesh_2d",
        "//darparu/renderer/io_controls:simple_2d",
    ],
)

cc_binary(
    name = "triangle_2d",
    srcs = ["triangle_2d.cc"],
    deps = [
        "//darparu/renderer",
        "//darparu/renderer:algebra",
        "//darparu/renderer:projection_context",
        "//darparu/renderer/cameras:pan",
        "//darparu/renderer/entities:mesh_2d",
        "//darparu/renderer/io_controls:simple_2d",
    ],
)
