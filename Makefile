refresh:
	bazel build  //darparu/bin:darparu && bazel run //:refresh_compile_commands

clean:
	bazel clean --expunge

cpu:
	bazel run  //darparu/bin:darparu \
	 --subcommands -c opt

debug:
	bazel run  //darparu/bin:darparu \
	 --subcommands -c dbg

.PHONY: refresh clean cpu debug
