# Fuzzing walkthrough (do this in WSL2 / Ubuntu)

A small, guided exercise: fuzz a tiny C parser with AFL++, find the crash,
fix it, and confirm it's gone. Expect ~1-2 focused sessions.

## 0. One-time setup (if not done)
    sudo apt update
    sudo apt install -y afl++ clang gdb build-essential

## 1. Get this folder into WSL2
Copy the whole `fuzzing-project` folder into your Linux home, e.g. open the
folder in Windows, then in Ubuntu:
    cp -r /mnt/c/Users/DELL/Desktop/"Chinese uni scholarship"/fuzzing-project ~/
    cd ~/fuzzing-project
    chmod +x *.sh

## 2. (First run only) let the system hand crashes to AFL
    echo core | sudo tee /proc/sys/kernel/core_pattern
(If AFL later complains about CPU scaling, prefix commands with AFL_SKIP_CPUFREQ=1.)

## 3. Build the parser (AFL++ instrumentation + AddressSanitizer)
    ./build.sh
This produces ./parser . If `afl-clang-fast` isn't found, use `which afl-cc`
and edit build.sh to use that name.

## 4. Fuzz it
    ./run.sh
You'll get a live orange dashboard. Watch the "saved crashes" (a.k.a. uniq
crashes) number. For this bug it usually turns non-zero within seconds to a
few minutes. Let it run a little, then stop with Ctrl+C.

## 5. Look at a crash
Crashing inputs are saved in `out/default/crashes/`. Pick one and reproduce:
    ls out/default/crashes/
    ./reproduce.sh out/default/crashes/id:000000*
AddressSanitizer will print something like:
    ERROR: AddressSanitizer: stack-buffer-overflow ... in parse_line parser.c:28
That line is `key[i] = line[i];` -- the copy with no length check. A key of
16+ characters writes past the 16-byte `key` box. That is the bug AFL found.

## 6. Fix it and prove it
The fix is one line -- add a bound so we stop before overflowing `key`:
    while (i < (int)sizeof(key) - 1 && line[i] != '=' && line[i] != '\0' && line[i] != '\n') {
`parser_fixed.c` already has this change (see: `diff parser.c parser_fixed.c`).
Apply the fix to parser.c (or copy parser_fixed.c over parser.c), then:
    ./build.sh
    ./run.sh
Let it run longer this time; "saved crashes" should stay 0. That's your proof
the bug is fixed.

## 7. Write it up
Fill in README_template.md with: the ASan output you saw, a screenshot of the
AFL dashboard, the root cause in your own words, and the one-line fix. Then
push the folder to GitHub (new repo, e.g. "afl-parser-fuzzing").

## Troubleshooting
- "No instrumentation detected" -> you compiled with plain gcc/clang, not
  afl-clang-fast. Rebuild with ./build.sh.
- AFL exits about memory -> keep the `-m none` already in run.sh.
- Nothing crashes after 10+ min -> check you built parser.c (the buggy one),
  not parser_fixed.c.
