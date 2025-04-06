load("@rules_cc//cc:defs.bzl", "cc_binary")

cc_binary(
    name = "darparu_birds_eye",
    srcs = ["darparu_birds_eye.cc"],
    deps = [
        "//darparu/renderer",
        "//darparu/renderer:algebra",
        "//darparu/renderer:io_control",
        "//darparu/renderer:projection_context",
        "//darparu/renderer/entities:container",
        "//darparu/renderer/entities:light",
        "//darparu/renderer/entities:plane",
        "//darparu/renderer/entities:water",
        "//darparu/renderer/io_controls:simple_3d",
    ],
)

cc_binary(
    name = "darparu",
    srcs = ["darparu.cc"],
    deps = [
        "//darparu/renderer",
        "//darparu/renderer/entities:ball",
        "//darparu/renderer/entities:container",
        "//darparu/renderer/entities:light",
        "//darparu/renderer/entities:water",
        "//darparu/renderer/io_controls:simple_3d",
    ],
)
