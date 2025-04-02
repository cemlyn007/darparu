load("@hedron_compile_commands//:refresh_compile_commands.bzl", "refresh_compile_commands")

refresh_compile_commands(
    name = "refresh_compile_commands",
    targets = {
        "//darparu/bin:darparu": "-c opt --//darparu:sycl=true --//darparu:device=nvidia",
    },
)
