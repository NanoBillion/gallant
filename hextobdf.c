/*
 * NAME
 *     hextobdf - convert font from hex format to bdf
 *
 * EXAMPLE USAGE
 *     hextobdf < gallant.hex > gallant.bdf
 *
 * LIMITATIONS
 *     Only for gallant font, due to hard-coded font/glyph properties.
 *     To adapt: modify PixelWidth and PixelHeight macros and
 *     output_bdf_preamble() and output_bdf_char() functions.
 */
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <inttypes.h>
#include <locale.h>
#include <wchar.h>
#include <unistd.h>

#ifndef VERSION
#define VERSION "(undefined)"
#endif

#define PixelWidth 12
#define PixelHeight 22
#define MAX_LINE 1024
#define MAX_GLYPHS 131072

struct glyph {
    wint_t  codepoint;
    char   *bitmap;
};

void    parse_options(int aArgc, char **aArgv);
void    usage(int aStatus);
void    errx(const char *aFormat, ...);
void    parse_font_dimensions(FILE *aFile);
void    parse_font_line(const char *aLine, struct glyph *aGlyph);
void    output_bdf_preamble(void);
void    output_bdf_char(int aChar);
void   *xmalloc(size_t aSize);

size_t  gWidth = PixelWidth;
size_t  gHeight = PixelHeight;
size_t  gLineNr = 0;
size_t  gBytes = 0;            // per one row of pixels in a regular glyph
size_t  gDblBytes = 0;         // per one row of pixels in a dbl width glyph
struct glyph *gGlyph = NULL;
int     gGlyphs = 0;

// start the ball rolling.
//
int main(int aArgc, char **aArgv) {
    if (!setlocale(LC_CTYPE, ""))
        errx("Can't set the locale. Check LANG, LC_CTYPE, LC_ALL.\n");
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
    output_bdf_preamble();
    for (int i = 0; i < gGlyphs; ++i) {
        output_bdf_char(i);
    }
    puts("ENDFONT");
    return EXIT_SUCCESS;
}

// Output the gallant BDF preamble.
//
void output_bdf_preamble(void) {
    puts("STARTFONT 2.1");
    puts("FONT -sun-gallant-medium-r-normal--22-220-75-75-C-120-ISO10646-1");
    puts("SIZE 22 75 75");
    puts("FONTBOUNDINGBOX 12 22 0 -5");
    puts("STARTPROPERTIES 18");
    puts("FONTNAME_REGISTRY \"\"");
    puts("FOUNDRY \"Sun\"");
    puts("FAMILY_NAME \"Gallant\"");
    puts("WEIGHT_NAME \"Medium\"");
    puts("SLANT \"R\"");
    puts("SETWIDTH_NAME \"Normal\"");
    puts("ADD_STYLE_NAME \"\"");
    puts("PIXEL_SIZE 22");
    puts("POINT_SIZE 220");
    puts("RESOLUTION_X 75");
    puts("RESOLUTION_Y 75");
    puts("SPACING \"C\"");
    puts("AVERAGE_WIDTH 120");
    puts("CHARSET_REGISTRY \"ISO10646\"");
    puts("CHARSET_ENCODING \"1\"");
    puts("FONT_ASCENT 17");
    puts("FONT_DESCENT 5");
    puts("DEFAULT_CHAR 65533");
    puts("ENDPROPERTIES");
    printf("CHARS %d\n", gGlyphs);
}

// Output data for a single STARTCHAR to stdout.
//
void output_bdf_char(int aChar) {
    size_t  hexdigits;
    printf("STARTCHAR U%04x\n", gGlyph[aChar].codepoint);
    printf("ENCODING %d\n", gGlyph[aChar].codepoint);
    if (wcwidth(gGlyph[aChar].codepoint) == 2) {
        puts("SWIDTH 1000 0\nDWIDTH 24 0\nBBX 24 22 0 -5");
        hexdigits = 2 * gDblBytes;
    }
    else {
        puts("SWIDTH 500 0\nDWIDTH 12 0\nBBX 12 22 0 -5");
        hexdigits = 2 * gBytes;
    }
    puts("BITMAP");
    const char *p = gGlyph[aChar].bitmap;
    for (size_t h = 0; h < gHeight; ++h) {
        for (size_t i = 0; i < hexdigits; ++i) {
            putchar(*p);
            ++p;
        }
        putchar('\n');
    }
    puts("ENDCHAR");
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
    fprintf(stderr, "usage: hextobdf [options]\n");
    fprintf(stderr, "Options [default]:\n");
    fprintf(stderr, "  -V             output version/hash and exit\n");
    fprintf(stderr, "  -h height      height in pixels [%d]\n", PixelHeight);
    fprintf(stderr, "  -w width       width in pixels [%d]\n", PixelWidth);
    fprintf(stderr, "\nReads hex font from stdin and writes bdf font to stdout\n");
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
