#ifndef STRING_H
#define STRING_H

static inline void* memset(void* buf, int c, int n)
{
    unsigned char* p = buf;

    while (n--) {
        *p++ = c;
    }

    return buf;
}

#endif
