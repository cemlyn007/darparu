refresh:
	bazel build  //darparu/bin:darparu && bazel run //:refresh_compile_commands

cpu:
	bazel run  //darparu/bin:darparu \
	 --subcommands -c opt

.PHONY: refresh cpu
