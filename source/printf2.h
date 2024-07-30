#ifndef PRINTF2_H
#define PRINTF2_H

#include <stdio.h>
#include <stdarg.h>
#include <string.h>

static
int parse_color(char * buf, char * fmt) {
    switch (*fmt) {
        case 'a': memcpy (buf, "\x1b[1;30m", 7ul); return 7; // gray
        case 'r': memcpy (buf, "\x1b[1;31m", 7ul); return 7; // red
        case 'g': memcpy (buf, "\x1b[1;32m", 7ul); return 7; // green
        case 'y': memcpy (buf, "\x1b[1;33m", 7ul); return 7; // yellow
        case 'b': memcpy (buf, "\x1b[1;34m", 7ul); return 7; // blue
        case 'p': memcpy (buf, "\x1b[1;35m", 7ul); return 7; // pink
        case 'c': memcpy (buf, "\x1b[1;36m", 7ul); return 7; // cyan
        case 'w': memcpy (buf, "\x1b[1;37m", 7ul); return 7; // white
        case '-': memcpy (buf, "\x1b[0m",    4ul); return 4; // reset
        case '@': memcpy (buf, "@",          1ul); return 1;
        default:                                   return 1;
    }
}

static
int parse_chars(char * buf, char * fmt) {
    memcpy (buf, fmt, 1ul);

    return 1;
}

static
void printf2(char * fmt, ...) {
    va_list args;
    char buf[1024] = "";

    for (int l = 0; *fmt != '\0'; ++fmt) {
        switch (*fmt) {
            case '@': l += parse_color(&buf[l], ++fmt); break;
            default:  l += parse_chars(&buf[l],   fmt); break;
        }
    }

    va_start(args, fmt);
    vprintf(buf, args);
    va_end(args);
}

#endif
