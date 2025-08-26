/*
 * NAME
 *     srctohex - convert font from src to hex format
 *
 * EXAMPLE USAGE
 *     srctohex -w 12 -h 22 < gallant.src > gallant.hex
 */
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <stdarg.h>
#include <locale.h>
#include <unistd.h>
#include <wchar.h>

#define PixelWidth 12
#define PixelHeight 22
#define LINE_MAX_WCHAR 1024
#define FULL_BLOCK 0x2588
#define MAX_GLYPHS 131072

#define STARTCHAR 1
#define BITMAP    2
#define ENDCHAR   3

void    parse_options(int aArgc, char **aArgv);
void    usage(int aStatus);
void    errx(const char *aFormat, ...);
int     parse_startchar(const wchar_t *aLine);
void    parse_bitmap(const wchar_t *aLine, int aWidth);
void    parse_endchar(const wchar_t *aLine);
bool    seen(unsigned int aCodepoint);
bool    lookup_codepoint(unsigned int aCodepoint);
int     compare_codepoints(const void *aFirst, const void *aSecond);
void   *xmalloc(size_t aSize);

int     gWidth = PixelWidth;
int     gHeight = PixelHeight;
int     gLineNr = 0;
unsigned int gCodepoint = 0;
unsigned int *gSeen = NULL;
size_t  gGlyphs = 0;

int main(int aArgc, char **aArgv) {
    if (!setlocale(LC_CTYPE, "")) {
        fprintf(stderr, "Can't set the locale. Check LANG, LC_CTYPE, LC_ALL.\n");
        exit(EXIT_FAILURE);
    }
    gSeen = xmalloc(MAX_GLYPHS * sizeof *gSeen);
    memset(gSeen, 0, MAX_GLYPHS * sizeof *gSeen);
    int     expect = STARTCHAR;
    int     bitmaps = 0;
    int     width = 1;
    parse_options(aArgc, aArgv);
    printf("# Width: %d\n# Height: %d\n", gWidth, gHeight);
    wchar_t wbuf[LINE_MAX_WCHAR];
    while (fgetws(wbuf, LINE_MAX_WCHAR, stdin)) {
        ++gLineNr;
        switch (expect) {
        case STARTCHAR:
            width = parse_startchar(wbuf);
            expect = BITMAP;
            break;
        case BITMAP:
            parse_bitmap(wbuf, width);
            ++bitmaps;
            if (bitmaps == gHeight) {
                putchar('\n');
                expect = ENDCHAR;
            }
            break;
        case ENDCHAR:
            parse_endchar(wbuf);
            expect = STARTCHAR;
            bitmaps = 0;
            ++gGlyphs;
            break;
        default:
            break;
        }
    }
    if (expect != STARTCHAR)
        errx("line %d, glyph U+%04x: incomplete glyph due to early end-of-file\n", gLineNr, gCodepoint);
    fprintf(stderr, "found %zu glyphs\n", gGlyphs);
    return EXIT_SUCCESS;
}

// Parse a STARTCHAR directive.
//
int parse_startchar(const wchar_t *aLine) {
    if (swscanf(aLine, L"STARTCHAR U+%x", &gCodepoint) == 1) {
        if (seen(gCodepoint))
            errx("line %d: glyph U+%04x multiply defined\n", gLineNr, gCodepoint);
        printf("%04x:", gCodepoint);
        return wcwidth((wchar_t) gCodepoint);
    }
    errx("line %d: expected 'STARTCHAR U+xxxx', got %ls", gLineNr, aLine);
    return 0;
}

// Has this codepoint been seen already?
//
bool seen(unsigned int aCodepoint) {
    if (gGlyphs == 0)
        return false;
    if (lookup_codepoint(aCodepoint))
        return true;
    if (aCodepoint < gSeen[gGlyphs - 1])
        errx("line %d: unsorted input: codepoint U+%04x follows U+%04x\n", gLineNr, aCodepoint, gSeen[gGlyphs - 1]);
    gSeen[gGlyphs] = aCodepoint;
    return false;
}

// Return true if codepoint is in gSeen[], false otherwise.
//
bool lookup_codepoint(unsigned int aCodepoint) {
    const unsigned int *p = bsearch(&aCodepoint, gSeen, gGlyphs, sizeof *gSeen, compare_codepoints);
    return p != NULL;
}

// Comparison callback function for bsearch().
//
int compare_codepoints(const void *aFirst, const void *aSecond) {
    const int *first = aFirst, *second = aSecond;
    return *first - *second;
}

// Parse a |BITMAP| directive.
//
void parse_bitmap(const wchar_t *aLine, int aWidth) {
    const wchar_t *delim1 = wcschr(aLine, L'|');
    if (delim1 == NULL)
        errx("line %d: initial delimiter '|' not found; early ENDCHAR?\n", gLineNr);

    const wchar_t *delim2 = wcschr(delim1 + 1, L'|');
    if (delim2 == NULL)
        errx("line %d: final delimiter '|' not found in %ls", gLineNr, aLine);

    const int bits = (delim2 - delim1) - 1;
    if (aWidth == 2) {
        if (bits != (2 * gWidth))
            errx("line %d, glyph U+%04x: expected %d pixels bewteen || delimiters for double width glyph, found %d\n", gLineNr,
                 gCodepoint, 2 * gWidth, bits);
    }
    else {
        if (bits != gWidth)
            errx("line %d, glyph U+%04x: expected %d pixels bewteen || delimiters for normal width glyph, found %d\n", gLineNr,
                 gCodepoint, gWidth, bits);
    }

    int     hex = 0;
    aLine = delim1 + 1;
    for (int i = 0; i < bits; ++i) {
        const int rem = i % 4;
        if (aLine[i] == FULL_BLOCK)
            hex += (8 >> rem);
        else {
            if (aLine[i] != L' ')
                errx("line %d, glyph U+%04x: pixels must be SPACE or FULL BLOCK U+2588 '%lc', found '%lc'\n", gLineNr, gCodepoint,
                     FULL_BLOCK, aLine[i]);
        }
        if (rem == 3) {
            putchar("0123456789abcdef"[hex]);
            hex = 0;
        }
    }
    /* Pad to octet with zero bits, if any. */
    const int pad = 8 * ((bits + 7) / 8);
    for (int i = bits; i < pad; ++i) {
        if (i % 4 == 3) {
            putchar("0123456789abcdef"[hex]);
            hex = 0;
        }
    }
}

// Parse an ENDCHAR directive.
//
void parse_endchar(const wchar_t *aLine) {
    if (wcscmp(aLine, L"ENDCHAR\n") != 0)
        errx("line %d, glyph U+%04x: expected 'ENDCHAR', got %ls", gLineNr, gCodepoint, aLine);
}

// Parse the command line options.
//
void parse_options(int aArgc, char **aArgv) {
    int     ch;
    while ((ch = getopt(aArgc, aArgv, "w:h:")) != -1) {
        switch (ch) {
        case 'h':
            if (sscanf(optarg, "%d", &gHeight) != 1)
                errx("can't convert '%s' to height integer\n", optarg);
            break;
        case 'w':
            if (sscanf(optarg, "%d", &gWidth) != 1)
                errx("can't convert '%s' to width integer\n", optarg);
            break;
        default:
            usage(EXIT_FAILURE);
        }
    }
}

// Allocate memory and exit on failure.
//
void   *xmalloc(size_t aSize) {
    void   *const mem = malloc(aSize);
    if (mem == NULL)
        errx("failed to allocate %zu bytes\n", aSize);
    return mem;
}

// Output usage message and exit with status.
//
void usage(int aStatus) {
    fprintf(stderr, "usage: srctohex [options]\n");
    fprintf(stderr, "Options [default]:\n");
    fprintf(stderr, "  -h height      height in pixels [%d]\n", PixelHeight);
    fprintf(stderr, "  -w width       width in pixels [%d]\n", PixelWidth);
    exit(aStatus);
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
