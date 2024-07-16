#ifndef SAFETY_H
#define SAFETY_H

#define check(code) do { \
    if (code) { return 1; } \
} while (0)

#define checked_fwrite(src, size, n, file) do { \
    if(fwrite (src, size, n, file) != n) { \
        return 1; \
    } \
} while (0)

#endif
