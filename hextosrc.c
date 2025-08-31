/*
 * NAME
 *     hextosrc - convert font from hex format to src
 *
 * EXAMPLE USAGE
 *     hextosrc < gallant.hex > gallant.src
 *
 * LIMITATIONS
 *     Only for gallant font, due to hard-coded font/glyph properties.
 *     To adapt: modify PixelWidth and PixelHeight macros.
 */
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <inttypes.h>
#include <locale.h>
#include <wchar.h>
#include <unistd.h>

/* FreeBSD: devel/libunistring */
#include <uniname.h>

#ifndef VERSION
#define VERSION "(undefined)"
#endif

#define PixelWidth 12
#define PixelHeight 22
#define MAX_LINE 1024
#define MAX_GLYPHS 131072
#define FULL_BLOCK 0x2588

struct glyph {
    wint_t  codepoint;
    char   *bitmap;
};

void    parse_options(int aArgc, char **aArgv);
void    usage(int aStatus);
void    errx(const char *aFormat, ...);
void    parse_font_dimensions(FILE *aFile);
void    parse_font_line(const char *aLine, struct glyph *aGlyph);
void    output_src_char(int aChar);
void   *xmalloc(size_t aSize);
uint8_t hex_value(char aXdigit);

size_t  gWidth = PixelWidth;
size_t  gHeight = PixelHeight;
size_t  gLineNr = 0;
size_t  gBytes = 0;            // per one row of pixels in a regular glyph
size_t  gDblBytes = 0;         // per one row of pixels in a dbl width glyph
struct glyph *gGlyph = NULL;

// start the ball rolling.
//
int main(int aArgc, char **aArgv) {
    if (!setlocale(LC_CTYPE, ""))
        errx("Can't set the locale. Check LANG, LC_CTYPE, LC_ALL.\n");
    int     gGlyphs = 0;
    gGlyph = xmalloc(MAX_GLYPHS * sizeof *gGlyph);
    parse_options(aArgc, aArgv);
    parse_font_dimensions(stdin);
    char    buf[MAX_LINE];
    while (fgets(buf, MAX_LINE, stdin)) {
        ++gLineNr;
        parse_font_line(buf, &gGlyph[gGlyphs]);
        ++gGlyphs;
    }
    fprintf(stderr, "found %d glyphs\n", gGlyphs);
    for (int i = 0; i < gGlyphs; ++i) {
        output_src_char(i);
    }
    return EXIT_SUCCESS;
}

// Output data for a single STARTCHAR to stdout.
//
void output_src_char(int aChar) {
    char    name[UNINAME_MAX + 1];
    const char *const u = unicode_character_name((ucs4_t) gGlyph[aChar].codepoint, name);
    printf("STARTCHAR U%04x %s\n", gGlyph[aChar].codepoint, u ? u : "<no name>");

    const bool is_double = wcwidth(gGlyph[aChar].codepoint) == 2;
    const size_t pixels = is_double ? 2 * gWidth : gWidth;
    const char *p = gGlyph[aChar].bitmap;
    for (size_t h = gHeight; h > 0; --h) {
        printf("%02zu |", h);
        for (size_t i = 0; i < pixels; ++i) {
            uint8_t nybble = hex_value(p[i / 4]);
            putwchar((nybble & (8u >> (i % 4))) ? FULL_BLOCK : L' ');
        }
        p += is_double ? 2 * gDblBytes : 2 * gBytes;
        puts("|");
    }
    puts("ENDCHAR");
}

// Compute value of aXdigit.
//
uint8_t hex_value(char aXdigit) {
    uint8_t h = (uint8_t) aXdigit;
    if ((h >= '0') && (h <= '9'))
        return h - '0';
    if ((h >= 'A') && (h <= 'F'))
        return (h - 'A') + 10;
    if ((h >= 'a') && (h <= 'f'))
        return (h - 'a') + 10;
    return 0xFFu;
}

// Parse one line of font hex data and store result in glyph.
//
void parse_font_line(const char *aLine, struct glyph *aGlyph) {
    char    c;
    if (sscanf(aLine, "%" SCNx32 "%c", &aGlyph->codepoint, &c) == 2 && c == ':') {
        const char *colon = strchr(aLine, ':');
        const size_t hexlen = strlen(colon + 1) - 1;    // Minus newline.

        if (wcwidth(aGlyph->codepoint) == 2) {
            if (hexlen != gHeight * gDblBytes * 2)
                errx("line %d: expected %zu hexdigits for double width glyph, got %zu\n", gLineNr, gHeight * gDblBytes * 2, hexlen);
        }
        else {
            if (hexlen != gHeight * gBytes * 2)
                errx("line %d: expected %zu hexdigits for normal width glyph, got %zu\n", gLineNr, gHeight * gBytes * 2, hexlen);
        }
        aGlyph->bitmap = xmalloc(hexlen);
        memcpy(aGlyph->bitmap, colon + 1, hexlen);
    }
    else
        errx("expected codepoint:hexdata in line %d\n", gLineNr);
}

// Parse the command line options.
//
void parse_options(int aArgc, char **aArgv) {
    int     ch;
    while ((ch = getopt(aArgc, aArgv, "Vw:h:")) != -1) {
        switch (ch) {
        case 'V':
            printf("%s version %s\n", aArgv[0], VERSION);
            exit (EXIT_SUCCESS);
            break;
        case 'h':
            if (sscanf(optarg, "%zu", &gHeight) != 1)
                errx("can't convert '%s' to height integer\n", optarg);
            break;
        case 'w':
            if (sscanf(optarg, "%zu", &gWidth) != 1)
                errx("can't convert '%s' to width integer\n", optarg);
            break;
        default:
            usage(EXIT_FAILURE);
        }
    }
    if (gWidth != PixelWidth || gHeight != PixelHeight)
        errx("dimensions do not match gallant font's 12x22\n");
}

// Parse the font's Width: and Height: directives.
//
void parse_font_dimensions(FILE *aFile) {
    char    line[MAX_LINE] = { 0 };
    for (int i = 1; i <= 2; ++i) {
        if (fgets(line, sizeof line, aFile) != NULL) {
            if (sscanf(line, " # Width: %zu", &gWidth) != 1)
                if (sscanf(line, " # Height: %zu", &gHeight) != 1)
                    errx("line %d must be '# Width or Height: number'\n", i);
        }
        else
            errx("could not read line %d\n", i);
    }
    if (gWidth != PixelWidth || gHeight != PixelHeight)
        errx("dimensions do not match gallant font's 12x22\n");
    gBytes = (gWidth + 7) / 8;
    gDblBytes = (2 * gWidth + 7) / 8;
}


// Output usage message and exit with status.
//
void usage(int aStatus) {
    fprintf(stderr, "usage: hextosrc [options]\n");
    fprintf(stderr, "Options [default]:\n");
    fprintf(stderr, "  -h height      height in pixels [%d]\n", PixelHeight);
    fprintf(stderr, "  -w width       width in pixels [%d]\n", PixelWidth);
    fprintf(stderr, "\nReads hex font from stdin and writes src font to stdout\n");
    exit(aStatus);
}

// Allocate memory and exit on failure.
//
void   *xmalloc(size_t aSize) {
    void   *const mem = malloc(aSize);
    if (mem == NULL)
        errx("failed to allocate %zu bytes\n", aSize);
    return mem;
}

// Print formatted message on stderr and exit.
//
void errx(const char *aFormat, ...) {
    va_list ap;
    va_start(ap, aFormat);
    vfprintf(stderr, aFormat, ap);
    va_end(ap);
    exit(EXIT_FAILURE);
}

/* vim: set tabstop=4 shiftwidth=4 expandtab fileformat=unix: */
