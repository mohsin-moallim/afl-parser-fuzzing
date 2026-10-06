#!/bin/sh
# Fuzz it. -m none: AddressSanitizer needs lots of virtual memory, so no limit.
# @@ is where AFL++ puts each test input file.
afl-fuzz -i seeds -o out -m none -- ./parser @@
