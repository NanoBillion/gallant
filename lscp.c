/*
 * NAME
 *    lscp - list a range of codepoints with width and name
 *
 * EXAMPLE USAGE
 *    $ lscp 0x2a00 0x2a10
 *    U+2a00  1 a ⨀ b N-ARY CIRCLED DOT OPERATOR
 *    U+2a01  1 a ⨁ b N-ARY CIRCLED PLUS OPERATOR
 *    U+2a02  1 a ⨂ b N-ARY CIRCLED TIMES OPERATOR
 *    U+2a03  1 a ⨃ b N-ARY UNION OPERATOR WITH DOT
 *    U+2a04  1 a ⨄ b N-ARY UNION OPERATOR WITH PLUS
 *    U+2a05  1 a ⨅ b N-ARY SQUARE INTERSECTION OPERATOR
 *    U+2a06  1 a ⨆ b N-ARY SQUARE UNION OPERATOR
 *    U+2a07  1 a ⨇ b TWO LOGICAL AND OPERATOR
 *    U+2a08  1 a ⨈ b TWO LOGICAL OR OPERATOR
 *    U+2a09  1 a ⨉ b N-ARY TIMES OPERATOR
 *    U+2a0a  1 a ⨊ b MODULO TWO SUM
 *    U+2a0b  1 a ⨋ b SUMMATION WITH INTEGRAL
 *    U+2a0c  1 a ⨌ b QUADRUPLE INTEGRAL OPERATOR
 *    U+2a0d  1 a ⨍ b FINITE PART INTEGRAL
 *    U+2a0e  1 a ⨎ b INTEGRAL WITH DOUBLE STROKE
 *    U+2a0f  1 a ⨏ b INTEGRAL AVERAGE WITH SLASH
 */
#include <stdio.h>
#include <stdlib.h>
#include <locale.h>
#include <wchar.h>
#include <errno.h>

#ifndef HASH
#define HASH "(undefined)"
#endif
#ifndef VERSION
#define VERSION "(undefined)"
#endif

/* FreeBSD: devel/libunistring */
#include <uniname.h>

int main(int aArgc, char **aArgv) {
    if (!setlocale(LC_CTYPE, "")) {
        fprintf(stderr, "Can't set the locale. Check LANG, LC_CTYPE, LC_ALL.\n");
        exit(EXIT_FAILURE);
    }
    if (aArgc != 3) {
        fprintf(stderr, "%s version %s, hash %s\n", aArgv[0], VERSION, HASH);
        fprintf(stderr, "usage: %s start end\n", aArgv[0]);
        exit(EXIT_FAILURE);
    }

    errno = 0;
    unsigned long start = strtoul(aArgv[1], NULL, 0);
    unsigned long end = strtoul(aArgv[2], NULL, 0);
    if (errno != 0) {
        fprintf(stderr, "could not convert arguments to integers\n");
        exit(EXIT_FAILURE);
    }

    for (unsigned long i = start; i < end; ++i) {
        char    name[UNINAME_MAX + 1];
        const char *const p = unicode_character_name((ucs4_t)i, name);
        printf("U+%04lx %2d a %lc b %s\n", i, wcwidth((wchar_t)i), (wint_t)i, p ? p : "<no name>");
    }
    return EXIT_SUCCESS;
}

/* vim: set syntax=c tabstop=4 shiftwidth=4 expandtab fileformat=unix: */
