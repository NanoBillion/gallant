/*
 * txttopng - create b/w png image from text file and raster font in hex format
 *
 * == USAGE
 * See parse_options() below.
 *
 * == FEATURES
 * • Combining characters work (if font contains them).
 * • Double width characters work (if font contains them).
 *
 * == AUTHOR
 * Jens Schweikhardt, 2025
 */
#include <stdbool.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <ctype.h>
#include <inttypes.h>
#include <errno.h>
#include <locale.h>
#include <wchar.h>
#include <unistd.h>

/* From libpng; on FreeBSD: /usr/ports/graphics/png. */
#include <png.h>

/* Default option values. */
#define TextFilename  "input.txt"
#define FontFilename  "jsgallant.hex"
#define PngFilename   "output.png"
#define InvertedImage false
#define Tabstop       8

/* That's hopefully plenty. */
#define MAX_GLYPHS    65536

/* Longest line in font file we want to parse. */
#define MAX_LINE      4096

// Glyph properties.
struct glyph {
    wint_t  codepoint;
    char   *bitmap;
    int     cells;
};

void    parse_options(int aArgc, char **aArgv);
void    load_font(void);
void    parse_font_dimensions(FILE *aFile);
void    parse_font_hexdata(FILE *aFile);
void    parse_font_line(const char *aLine, int aLineNr, struct glyph *aGlyph);
void    set_replacement_character(void);
int     count_glyphs(FILE *aFile);
void    load_text(void);
void    slurp_text(FILE *aFile);
void    fb_alloc(int aHeight, int aWidth, int aRows, int aColumns, bool aInverted);
void    fb_draw_pixel(int aXpos, int aYpos);
void    fb_draw_glyph(wint_t aCodepoint, int aRow, int aColumn);
void    fb_draw_text(void);
void    fb_save_png(void);
struct glyph *lookup_glyph(wint_t aCodepoint);
int     compare_glyphs(const void *aFirst, const void *aSecond);
FILE   *xfopen(const char *aFilename, const char *aMode);
void   *xmalloc(size_t aSize);
void    errx(const char *aFormat, ...);
uint8_t hex_value(uint8_t aXdigit);
void    usage(int aStatus);

// Array of frame buffer scan lines ("rows" in PNG parlance).
static png_bytep *gFramebuffer = NULL;

// Rasterfont storage and properties.
static struct glyph *gGlyphset;
static int gGlyphs = 0;
static int gWidth = 0;
static int gHeight = 0;
static int gBytes = 0;         // per one row of pixels in a regular glyph
static int gDblBytes = 0;      // per one row of pixels in a dbl width glyph
static struct glyph *gReplacement = NULL;

// Input text storage and properties.
static wchar_t *gText = NULL;
static size_t gTextChars = 0;
static int gRows = 0;
static int gColumns = 0;
static int gTabstop = Tabstop;

// Default options.
static char *gTextFilename = TextFilename;
static char *gFontFilename = FontFilename;
static char *gPngFilename = PngFilename;
static bool gInverted = InvertedImage;

// Start the ball rolling.
//
int main(int aArgc, char **aArgv) {
    if (!setlocale(LC_CTYPE, ""))
        errx("Can't set the locale. Check LANG, LC_CTYPE, LC_ALL.\n");
    parse_options(aArgc, aArgv);
    load_text();
    load_font();
    fb_alloc(gHeight, gWidth, gRows, gColumns, gInverted);
    fb_draw_text();
    fb_save_png();
    return EXIT_SUCCESS;
}

// Parse the command line options.
//
void parse_options(int aArgc, char **aArgv) {
    int     ch;
    while ((ch = getopt(aArgc, aArgv, "f:hip:T:t:")) != -1) {
        switch (ch) {
        case 'f':
            gFontFilename = optarg;
            break;
        case 'h':
            usage(EXIT_SUCCESS);
            break;
        case 'i':
            gInverted = true;
            break;
        case 'p':
            gPngFilename = optarg;
            break;
        case 'T':
            if (sscanf(optarg, "%d", &gTabstop) != 1)
                errx("can't convert '%s' to tabstop integer\n", optarg);
            break;
        case 't':
            gTextFilename = optarg;
            break;
        default:
            usage(EXIT_FAILURE);
        }
    }
}

// Output usage message and exit with status.
//
void usage(int aStatus) {
    fprintf(stderr, "usage: txttopng [options]\n");
    fprintf(stderr, "Options [default]:\n");
    fprintf(stderr, "  -h             show this help text\n");
    fprintf(stderr, "  -i             inverts image to black on white [%s]\n", InvertedImage ? "true" : "false");
    fprintf(stderr, "  -f fontfile    [%s]\n", FontFilename);
    fprintf(stderr, "  -p pngfile     [%s]\n", PngFilename);
    fprintf(stderr, "  -T tabstop     [%d]\n", Tabstop);
    fprintf(stderr, "  -t textfile    [%s]\n", TextFilename);
    exit(aStatus);
}

// Print the gText[] array glyph by glyph to the frame buffer.
//
void fb_draw_text(void) {
    int     row = 0;
    int     col = 0;
    for (size_t i = 0; i < gTextChars; ++i) {
        switch (wcwidth(gText[i])) {
        case -1:
            switch (gText[i]) {
            case L'\t':
                col += gTabstop;
                col -= (col % gTabstop);
                break;
            case L'\n':
                ++row;
                col = 0;
                break;
            case L'\v':
            case L'\f':
                /* Handle \v and \f like xterm: advance to next row. */
                ++row;
                break;
            case L'\r':
                col = 0;
                break;
            default:
                break;
            }
            break;
        case 0:
            fb_draw_glyph(gText[i], row, col > 0 ? col - 1 : 0);
            break;
        case 1:
            fb_draw_glyph(gText[i], row, col);
            ++col;
            break;
        case 2:
            fb_draw_glyph(gText[i], row, col);
            col += 2;
            break;
        default:
            break;
        }
    }
}

// Load utf8 encoded text from file into gText[].
//
void load_text(void) {
    FILE   *const fp = xfopen(gTextFilename, "rb");
    size_t  wchars = 0;
    int     column = 0;

    gRows = 0;
    gColumns = 0;
    wint_t  wc;
    while ((wc = fgetwc(fp)) != WEOF) {
        ++wchars;
        switch (wcwidth(wc)) {
        case -1:
            /* Control character. A few influence row and column. */
            if (wc == L'\t') {
                column += gTabstop;
                column -= (column % gTabstop);
            }
            else if (wc == L'\n') {
                ++gRows;
                if (column > gColumns)
                    gColumns = column;
                column = 0;
            }
            else if (wc == L'\v' || wc == L'\f') {
                /* Handle \v and \f like xterm: advance to next row. */
                ++gRows;
            }
            else if (wc == L'\r') {
                column = 0;
            }
            else
                fprintf(stderr, "ignoring width=-1 character U+%04x in row %d\n", wc, gRows + 1);
            break;
        case 0:
            /* Combining character, zero width space, ... */
            break;
        case 1:
            /* Ordinary character. */
            ++column;
            break;
        case 2:
            /* Double width character. */
            column += 2;
            break;
        default:
            errx("unexpected wcwidth(U+%04x)=%d\n", (unsigned int) wc, wcwidth(wc));
            break;
        }
    }
    gTextChars = wchars;
    printf("found %zu codepoints in %s, %d rows, max %d colums\n", gTextChars, gTextFilename, gRows, gColumns);
    slurp_text(fp);
    fclose(fp);
}

// Read text into gTextChars[] array.
//
void slurp_text(FILE *aFile) {
    rewind(aFile);
    gText = xmalloc(gTextChars * sizeof *gText);
    for (size_t i = 0; i < gTextChars; ++i) {
        const wint_t wc = fgetwc(aFile);
        if (wc != WEOF)
            gText[i] = (wchar_t) wc;
        else
            gText[i] = L'\ufffd';   // File got shorter than expected.
    }
}

// Save the frame buffer as a PNG image.
//
void fb_save_png(void) {
    FILE   *fp = xfopen(gPngFilename, "wb");
    png_structp png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    if (!png_ptr)
        errx("png_create_write_struct failed\n");

    png_infop info_ptr = png_create_info_struct(png_ptr);
    if (!info_ptr)
        errx("png_create_write_struct failed\n");

    if (setjmp(png_jmpbuf(png_ptr)))
        errx("fatal png error\n");

    png_init_io(png_ptr, fp);
    png_set_IHDR(png_ptr, info_ptr, gWidth * gColumns, gHeight * gRows, 1,
                 PNG_COLOR_TYPE_GRAY, PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);
    png_write_info(png_ptr, info_ptr);
    png_write_image(png_ptr, gFramebuffer);
    png_write_end(png_ptr, NULL);
    png_destroy_write_struct(&png_ptr, &info_ptr);
    fclose(fp);
    printf("wrote WxH = %dx%d image to %s\n", gWidth * gColumns, gHeight * gRows, gPngFilename);
}

// Allocate frame buffer to hold the pixels. White on black, unless inverted.
//
void fb_alloc(int aHeight, int aWidth, int aRows, int aColumns, bool aInverted) {
    const int fb_lines = aHeight * aRows;
    gFramebuffer = xmalloc(fb_lines * sizeof *gFramebuffer);

    const int fb_pixels_per_line = aWidth * aColumns;
    const int fb_bytes_per_line = (fb_pixels_per_line + 7) / 8;
    for (int line = 0; line < fb_lines; ++line) {
        gFramebuffer[line] = xmalloc(fb_bytes_per_line);
        memset(gFramebuffer[line], aInverted ? 0xFF : 0, fb_bytes_per_line);
    }
}

// Load font in hex format from gFontFilename.
//
void load_font(void) {
    FILE   *fp = xfopen(gFontFilename, "r");

    parse_font_dimensions(fp);
    gGlyphs = count_glyphs(fp);
    fprintf(stderr, "found %d glyphs, width %d, height %d in %s\n", gGlyphs, gWidth, gHeight, gFontFilename);
    if (gGlyphs < 2)
        errx("that's not a font, it would seem\n");
    gGlyphset = xmalloc(gGlyphs * sizeof *gGlyphset);
    rewind(fp);
    parse_font_hexdata(fp);
    fclose(fp);
}

// Parse the font's Width: and Height: directives.
//
void parse_font_dimensions(FILE *aFile) {
    char    line[MAX_LINE] = { 0 };
    for (int i = 1; i <= 2; ++i) {
        if (fgets(line, sizeof line, aFile) != NULL) {
            if (sscanf(line, " # Width: %d", &gWidth) != 1)
                if (sscanf(line, " # Height: %d", &gHeight) != 1)
                    errx("line %d in %s must be '# Width or Height: number'\n", i, gFontFilename);
        }
        else
            errx("could not read line %d in %s\n", i, gFontFilename);
    }
    if (gWidth <= 0 || gHeight <= 0)
        errx("could not parse width or height at the start of %s\n", gFontFilename);
    gBytes = (gWidth + 7) / 8;
    gDblBytes = (2 * gWidth + 7) / 8;
}

// First pass: count glyphs.
//
int count_glyphs(FILE *aFile) {
    int     glyphs = 0;
    char    line[MAX_LINE] = { 0 };
    int     line_no = 2;
    while (fgets(line, sizeof line, aFile) != NULL) {
        char    c;
        ++line_no;
        if (line[0] == '#')
            continue;
        if (sscanf(line, "%*" SCNx32 "%c", &c) == 1 && c == ':')
            ++glyphs;
        else
            fprintf(stderr, "skipping line %d in %s, does not scan\n", line_no, gFontFilename);
        if (glyphs > MAX_GLYPHS)
            errx("too many glyphs (max %d) in %s\n", MAX_GLYPHS, gFontFilename);
    }
    return glyphs;
}

// Load and convert hex data to binary bitmap representation.
//
void parse_font_hexdata(FILE *aFile) {
    int     glyphs = 0;
    int     line_no = 0;
    char    line[MAX_LINE] = { 0 };

    while (fgets(line, sizeof line, aFile) != NULL) {
        ++line_no;
        if (line[0] == '#')
            continue;
        parse_font_line(line, line_no, &gGlyphset[glyphs]);
    }
    if (gGlyphs != glyphs)
        errx("glyph count changed unexpectedly\n");
    qsort(gGlyphset, gGlyphs, sizeof *gGlyphset, compare_glyphs);
    set_replacement_character();
}

// Assign a suitable replacement character. If none was in the font, use 50%
// shade made of vertical 1 pixel bars. That works for any size font.
//
void set_replacement_character(void) {
    gReplacement = NULL;
    gReplacement = lookup_glyph(0xfffd);
    if (gReplacement != NULL)
        return;
    gReplacement = xmalloc(sizeof *gReplacement);
    gReplacement->bitmap = xmalloc(gHeight * gBytes);
    memset(gReplacement->bitmap, 0xaa, gHeight * gBytes);
    gReplacement->codepoint = 0xfffd;
    gReplacement->cells = 1;
}

// Parse one line of font hex data and store result in glyph.
//
void parse_font_line(const char *aLine, int aLineNr, struct glyph *aGlyph) {
    char    c;
    uint32_t codepoint;
    if (sscanf(aLine, "%" SCNx32 "%c", &codepoint, &c) == 2 && c == ':') {
        const char *colon = strchr(aLine, ':');
        const int len = (int) strlen(colon + 1);
        int     bytes = gHeight;
        if (len == gHeight * gDblBytes * 2 + 1)
            bytes *= gDblBytes;
        else if (len == gHeight * gBytes * 2 + 1)
            bytes *= gBytes;
        else
            errx("unexpected length of hex data\n");

        aGlyph->codepoint = (wint_t) codepoint;
        aGlyph->bitmap = xmalloc(bytes);
        aGlyph->cells = (bytes == gHeight * gBytes ? 1 : 2);
        // Convert nybbles to bytes.
        const char *hex = colon + 1;
        for (int i = 0; i < 2 * bytes; i += 2) {
            if (!isxdigit(hex[i]) || !isxdigit(hex[i + 1]))
                errx("invalid byte '%c%c' in file %s, line %d\n", hex[i], hex[i + 1], gFontFilename, aLineNr);
            aGlyph->bitmap[i / 2] = (hex_value(hex[i]) << 4) + hex_value(hex[i + 1]);
        }
    }
    else
        errx("expected codepoint:hexdata in file %s, line %d\n", gFontFilename, aLineNr);
}

// Compute value of aXdigit.
//
uint8_t hex_value(uint8_t aXdigit) {
    if ((aXdigit >= '0') && (aXdigit <= '9'))
        return aXdigit - '0';
    if ((aXdigit >= 'A') && (aXdigit <= 'F'))
        return (aXdigit - 'A') + 10;
    if ((aXdigit >= 'a') && (aXdigit <= 'f'))
        return (aXdigit - 'a') + 10;
    return 0xFFu;
}

// Compare callback for qsort and bsearch.
//
int compare_glyphs(const void *aFirst, const void *aSecond) {
    const struct glyph *first = aFirst, *second = aSecond;
    return first->codepoint - second->codepoint;
}

// Draw a codepoint's glyph into the frame buffer at the given position.
//
void fb_draw_glyph(wint_t aCodepoint, int aRow, int aColumn) {
    const struct glyph *g = lookup_glyph(aCodepoint);
    const char *bitmap = g->bitmap;
    int     ypos = gHeight * aRow;
    for (int i = 0; i < gHeight; ++i) {
        int     xpos = gWidth * aColumn;
        uint8_t mask = 128;
        for (int p = 0; p < gWidth * g->cells; ++p) {
            // get pixel p from bitmap
            // byte = p/8; bit = 7 - p%8
            if (bitmap[p / 8] & mask)
                fb_draw_pixel(xpos, ypos);
            if ((mask >>= 1) == 0)
                mask = 128;
            ++xpos;
        }
        bitmap += (g->cells == 1) ? gBytes : gDblBytes;
        ++ypos;
    }
}

// Set pixel (aXpos,aYpos) in the frame buffer.
//
void fb_draw_pixel(int aXpos, int aYpos) {
    const uint8_t mask = 1u << (7 - (aXpos % 8));
    if (gInverted)
        gFramebuffer[aYpos][aXpos / 8] &= ~mask;
    else
        gFramebuffer[aYpos][aXpos / 8] |= mask;
}

// Return pointer to glyph data or of the replacement character.
//
struct glyph *lookup_glyph(wint_t aCodepoint) {
    struct glyph key = { 0 };

    key.codepoint = aCodepoint;
    struct glyph *const p = bsearch(&key, gGlyphset, gGlyphs, sizeof *gGlyphset, compare_glyphs);
    return p != NULL ? p : gReplacement;
}

// Open file and exit on failure.
//
FILE   *xfopen(const char *aFilename, const char *aMode) {
    FILE   *const fp = fopen(aFilename, aMode);
    if (fp == NULL)
        errx("can't open(%s,%s): %s\n", aFilename, aMode, strerror(errno));
    return fp;
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

/* vim: set syntax=c tabstop=4 shiftwidth=4 expandtab fileformat=unix: */
