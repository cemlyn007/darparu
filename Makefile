refresh:
	bazel run -c opt --subcommands //:refresh_compile_commands

cpu:
	bazel run  //darparu/bin:darparu \
	 --subcommands -c opt

.PHONY: refresh cpu
