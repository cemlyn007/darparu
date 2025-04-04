load("@rules_cc//cc:defs.bzl", "cc_binary")

cc_binary(
    name = "darparu",
    srcs = ["darparu.cc"],
    deps = [
        "//darparu/renderer",
        "//darparu/renderer/entities:ball",
        "//darparu/renderer/entities:container",
        "//darparu/renderer/entities:light",
        "//darparu/renderer/entities:water",
    ],
)
