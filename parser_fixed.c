/*
 * parser.c  --  a tiny, deliberately buggy parser, for learning fuzzing.
 *
 * It reads a file of "key=value" lines and prints each key and value.
 * It stands in for any program that reads untrusted input (like the
 * network data your NIDS reads).
 *
 * There is ONE intentional bug (a missing length check) for AFL++ to find.
 * Your job in this exercise: fuzz it, watch AFL++ find the crash, understand
 * it, then fix it (see parser_fixed.c) and fuzz again to confirm it's gone.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Parse one "key=value" line. */
void parse_line(const char *line) {
    char key[16];          /* a fixed-size box: holds at most 15 chars + terminator */
    int i = 0;

    /* Copy everything before '=' into key.
     *
     * THE BUG: we never check that i stays inside key[16].
     * If the key part is 16 characters or longer, we write past the end
     * of the box. That is a "buffer overflow" -- exactly what AFL++ finds.
     */
    while (i < (int)sizeof(key) - 1 && line[i] != '=' && line[i] != '\0' && line[i] != '\n') {
        key[i] = line[i];
        i++;
    }
    key[i] = '\0';

    const char *value = "";
    if (line[i] == '=') {
        value = &line[i + 1];
    }

    printf("key=%s value=%s\n", key, value);
}

int main(int argc, char **argv) {
    if (argc < 2) {
        fprintf(stderr, "usage: %s <input-file>\n", argv[0]);
        return 1;
    }
    FILE *f = fopen(argv[1], "r");
    if (!f) { perror("fopen"); return 1; }

    char line[256];
    while (fgets(line, sizeof(line), f) != NULL) {
        parse_line(line);
    }
    fclose(f);
    return 0;
}
