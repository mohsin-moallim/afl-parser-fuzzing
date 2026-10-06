#!/bin/sh
# Compile the parser for fuzzing: AFL++ instrumentation + AddressSanitizer.
AFL_USE_ASAN=1 afl-clang-fast -g -O1 parser.c -o parser
echo "built ./parser"
