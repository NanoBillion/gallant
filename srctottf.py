#!/usr/bin/env python
"""
Convert gallant.src to gallant.ttf (a TrueType font)
"""

import sys
import random
from fontTools.fontBuilder import FontBuilder
from fontTools.pens.ttGlyphPen import TTGlyphPen
from fontTools.ttLib.tables._g_a_s_p import table__g_a_s_p

if sys.version_info[0] < 3:
    sys.stderr.write("Error: This script requires Python 3.\n")
    sys.exit(1)

def read_src_file(filename):
    """
    Read the font src file and fill the glyph[codepoint] dict.
    Flip image upside down so x,y origin is at the bottom left.
    This makes generating TrueType paths easy, but generating SVG
    (which has the origin at the top left) slightly harder.
    @param   filename      File name.
    @return  Dict of glyph parameters.
    @return  Keys are the numeric codepoints.
    @return  Values are glyph parameters { "width", "name", "lsb", "contour" }.
    """
    (glyph, src, codepoint, name) = ({}, [], -1, "")
    with open(filename, "r", encoding="utf-8") as f:
        for line in f:
            if line.startswith("STARTCHAR"):
                src = []
                ucp, name = line.split(maxsplit=2)[1:3]
                codepoint = int(ucp[1:], 16)
            elif line.startswith("ENDCHAR"):
                src.reverse()
                width = len(src[0])
                pixels = set((col, row - DESCENT) \
                    for row, l in enumerate(src) \
                    for col, p in enumerate(l) if p != ' ')
                glyph[codepoint] = {
                    "width":   width,
                    "name":    name[:-1],
                    "lsb":     min(width - len(s.lstrip()) for s in src),
                    "contour": find_contours(find_edges(pixels))
                }
            else:
                src.append(line[4:len(line) - 2])
    return glyph

def find_edges(pixel):
    """
    Compute edges from set of pixels.
    An edge is a coordinate pair forming a unit vector up, down, left or right.
    Clockwise for outer contours.
    @param   pixel        List of set pixels.
    @return  List of edges.
    """
    edge = []
    for (x, y) in pixel:
        if (x, y - 1) not in pixel: # Bottom edge: left
            edge.append(((x+1, y), (x, y)))
        if (x + 1, y) not in pixel: # Right edge: down
            edge.append(((x+1, y+1), (x+1, y)))
        if (x, y + 1) not in pixel: # Top edge: right
            edge.append(((x, y+1), (x+1, y+1)))
        if (x - 1, y) not in pixel: # Left edge: up
            edge.append(((x, y), (x, y+1)))
    return edge

def find_contours(edge):
    """
    Find contours in list of edges.
    A contour is a sequence of edges forming a closed and oriented stroke.
    @param   edge        List of edges.
    @return  List of optimized contours.
    """
    adj = {}
    for start, end in sorted(edge):
        adj.setdefault(start, []).append(end)

    contour = []
    while adj:
        start_edge = next(iter(adj.keys()))
        curr_edge = start_edge
        curr_contour = [curr_edge]
        while curr_edge in adj:
            next_options = adj[curr_edge]
            next_edge = next_options.pop()
            if not next_options:
                del adj[curr_edge]
            if next_edge == start_edge:
                break
            curr_contour.append(next_edge)
            curr_edge = next_edge
        contour.append(optimize_contour(curr_contour))
    return contour

def optimize_contour(contour):
    """
    Eliminate intermediate points.
    @param   contour       Sequence of edges forming a closed path.
    @return  Optimized contour as a list of coordinates (2-tuples).
    """
    points = len(contour)
    if points < 6:
        return contour
    result = []
    for i in range(points):
        pred = contour[(i - 1) % points]
        this = contour[i]
        succ = contour[(i + 1) % points]
        if pred[1] == this[1] == succ[1]:
            continue
        if pred[0] == this[0] == succ[0]:
            continue
        result.append(this)
    return result

def create_svg(contour, codepoint, scale):
    """
    Convert oriented contours to SVG.
    @param   contour     List of oriented closed paths.
    @param   codepoint   Glyph's numeric Unicode codepoint.
    @param   scale       Scale factor applied to all coordinates.
    @return  SVG document with one pixel padding and a bounding box.
    """
    svg_width = 2 * (1 + WIDTH + 1) * scale # Always full width.
    svg_height = (1 + ASCENT + DESCENT + 1) * scale
    bbx = (GLYPH[codepoint]["width"] + 1) * scale + 1 # Glyph bounding box.
    bby = (ASCENT + DESCENT + 1) * scale + 1

    svg = '<svg xmlns="http://www.w3.org/2000/svg" '
    svg += f'width="{svg_width}" height="{svg_height}" '
    svg += f'viewBox="0 0 {svg_width} {svg_height}" id="U{codepoint:04x}">\n'
    svg += f'<line stroke="black" x1="{scale}" y1="{(ASCENT+1)*scale}" '
    svg += f'x2="{bbx}" y2="{(ASCENT+1)*scale}" stroke-dasharray="1 1"/>\n'

    svg += '<path stroke="black" fill="lightgray" d="'
    for path in contour:
        for i, (x, y) in enumerate(path):
            command = "M" if i == 0 else " L"
            svg += f"{command} {(x+1)*scale},{(ASCENT-y+1)*scale}"
        svg += " Z\n"
    svg += '"/>\n'
    svg += colored_contours(contour, scale)
    svg += '<path stroke="red" stroke-dasharray="1 1" fill="none" '
    svg += f'd="M {scale-1},{scale-1} '
    svg += f' L {bbx},{scale-1} L {bbx},{bby} {scale-1},{bby} Z"/>\n'
    svg += f'<g font-family="Helvetica" font-size="{scale / 2}">\n'
    svg += f' <text x="{scale}" y="{scale * .75}">U+{codepoint:04X}</text>\n'
    svg += f' <text x="{scale}" y="-{scale/4}" transform="rotate(90)">'
    svg += f'{GLYPH[codepoint]["name"].replace("<","&lt;")}</text>\n'
    svg += '</g>\n</svg>\n'
    return svg

def colored_contours(contour, scale):
    """
    Draw each contour in a different color.
    @param   contour     List of contours.
    @param   scale       Scale factor.
    @return  SVG commands as a string.
    """
    svg = ''
    random.seed(2)
    for j, path in enumerate(contour):
        r = random.randint(0, 15)
        g = random.randint(0, 15)
        b = random.randint(0, 15)
        color = f"#{r:1x}{g:1x}{b:1x}"
        for i, (x, y) in enumerate(path):
            if i == 0:
                svg += f'<text font-size="{scale / 2}" '
                svg += f'x="{(x+1)*scale+2}" y="{(ASCENT-y+1)*scale-2}">{j}</text>\n'
                svg += f'<circle cx="{(x+1)*scale}" cy="{(ASCENT-y+1)*scale}" '
                svg += f'r="3" fill="{color}"/>\n'
                svg += f'<path stroke="{color}" fill="none" d="'
                command = "M"
            else:
                command = " L"
            svg += f"{command} {(x+1)*scale},{(ASCENT-y+1)*scale}"
        svg += ' Z"/>\n'
    return svg

def create_true_type():
    """
    Create TrueType Font.  Save result in gallant.ttf.
    @param   cpaths      List of oriented closed paths.
    """
    fb = FontBuilder(unitsPerEm=2048, isTTF=True)

    # Glyph Order. See "Recommendations for OpenType Fonts"
    # https://learn.microsoft.com/en-us/typography/opentype/otspec160/recom
    # https://learn.microsoft.com/en-us/typography/opentype/otspec190/recom
    # https://learn.microsoft.com/en-us/typography/opentype/spec/recom
    glyph_order = [".notdef", ".null", "nonmarkingreturn", "space"]
    cmap = {
        0x0000: ".null",
        0x000D: "nonmarkingreturn",
        0x0020: "space"
    }

    # Add remaining codepoints to glyph_order and cmap.
    # Note: We explicitly skip U+FFFE and U+FFFF because
    # they are reserved Unicode non-characters.
    for codepoint in sorted(GLYPH.keys()):
        if codepoint in (0x0000, 0x000D, 0x0020, 0xFFFE, 0xFFFF):
            continue
        name = f"uni{codepoint:04X}"
        glyph_order.append(name)
        cmap[codepoint] = name
    fb.setupGlyphOrder(glyph_order)

    # Build Glyphs.
    glyphs = {}
    metrics = {}
    setup_hollow(glyphs, metrics, (".notdef",))
    setup_empty(glyphs, metrics, (".null", "nonmarkingreturn", "space"))

    # Draw remaining glyphs.
    for codepoint, glyph in GLYPH.items():
        if codepoint in (0x0000, 0x000D, 0x0020, 0xFFFE, 0xFFFF):
            continue
        name = cmap[codepoint]
        metrics[name] = (glyph["width"] * TT_PIXEL_SIZE, glyph["lsb"] * TT_PIXEL_SIZE)
        pen = TTGlyphPen(None)
        for contour in glyph["contour"]:
            for i, (x, y) in enumerate(contour):
                if i == 0:
                    pen.moveTo((x * TT_PIXEL_SIZE, y * TT_PIXEL_SIZE))
                else:
                    pen.lineTo((x * TT_PIXEL_SIZE, y * TT_PIXEL_SIZE))
            pen.closePath()
        glyphs[name] = pen.glyph()

    fb.setupGlyf(glyphs)
    fb.setupCharacterMap(cmap)
    fb.setupHorizontalMetrics(metrics)
    fb.setupHorizontalHeader(ascent=TT_ASCENT, descent=TT_DESCENT)
    setup_names(fb)
    setup_os2(fb)
    fb.font["gasp"] = gasp_table()
    fb.setupPost(keepGlyphNames=False)
    #fb.setupDummyDSIG()
    fb.font["post"].isFixedPitch = 1
    save_fonts(fb)

def setup_hollow(glyphs, metrics, namelist):
    """
    Setup glyphs that should be a hollow rectangle.
    @param   glyphs         Dictionary of glyphs.
    @param   metrics        Dictionary of metrics.
    @param   namelist       List of glyph names.
    """
    for name in namelist:
        pen = TTGlyphPen(None)
        pen.moveTo((0, TT_DESCENT))
        pen.lineTo((0, TT_ASCENT))
        pen.lineTo((WIDTH * TT_PIXEL_SIZE, TT_ASCENT))
        pen.lineTo((WIDTH * TT_PIXEL_SIZE, TT_DESCENT))
        pen.closePath()
        pen.moveTo((TT_PIXEL_SIZE, TT_DESCENT + TT_PIXEL_SIZE))
        pen.lineTo(((WIDTH - 1) * TT_PIXEL_SIZE, TT_DESCENT + TT_PIXEL_SIZE))
        pen.lineTo(((WIDTH - 1) * TT_PIXEL_SIZE, TT_ASCENT - TT_PIXEL_SIZE))
        pen.lineTo((TT_PIXEL_SIZE, TT_ASCENT - TT_PIXEL_SIZE))
        pen.closePath()
        glyphs[name] = pen.glyph()
        metrics[name] = (WIDTH * TT_PIXEL_SIZE, 0)

def setup_empty(glyphs, metrics, namelist):
    """
    Setup glyphs that should be empty.
    @param   glyphs         Dictionary of glyphs.
    @param   metrics        Dictionary of metrics.
    @param   namelist       List of glyph names.
    """
    for name in namelist:
        glyphs[name] = TTGlyphPen(None).glyph()
        metrics[name] = (WIDTH * TT_PIXEL_SIZE, 0)

def save_fonts(fb):
    """
    Save font as TTF, WOFF and WOFF2.
    @param   fb           Fontbuilder object."
    """
    fb.font.flavor = None
    fb.save("gallant.ttf")
    fb.font.flavor = "woff"
    fb.save("gallant.woff")
    fb.font.flavor = "woff2"
    fb.save("gallant.woff2")

def setup_names(fb):
    """
    Setup the name table.
    @param   fb           Fontbuilder object."
    """
    fb.setupNameTable({
        "copyright":            {"en": "Jens Schweikhardt - SPDX-License-Identifier: BSD-2-Clause"},
        "familyName":           {"en": FONT},
        "styleName":            {"en": STYLE},
        "uniqueFontIdentifier": {"en": f"{FONT} {STYLE}; {VERSION}"},
        "fullName":             {"en": f"{FONT} {STYLE}"},
        "version":              {"en": f"Version {VERSION}"},
        "psName":               {"en": f'{FONT.replace(" ", "")}-{STYLE}'},
        "manufacturer":         {"en": "Jens Schweikhardt"},
        "designer":             {"en": "Sun Microsystems PROM font (Latin-1): Vaughan Pratt"},
        "description":          {"en":
            "The Sun Microsystems Console Font (Pixel Raster WxH=12x22, Ascent 17, Descent 5)"},
        "vendorURL":            {"en": "https://github.com/NanoBillion/gallant"},
        "designerURL":          {"en": "https://github.com/NanoBillion/gallant"},
        "licenseDescription":   {"en": "SPDX-License-Identifier: BSD-2-Clause"},
        "licenseInfoURL":       {"en": "https://github.com/NanoBillion/gallant/blob/main/LICENSE"},
        "sampleText":
            {"en": "Probing /iommu@0,10000000/sbus@0,10001000 at 4.0 SUNW,CS4231 power-management"},
    })

def setup_os2(fb):
    """
    Setup the OS/2 table.
    @param   fb           Fontbuilder object."
    @note    https://learn.microsoft.com/en-us/typography/opentype/otspec182/os2
    """
    fb.setupOS2(
        sTypoAscender  =  TT_ASCENT,
        sTypoDescender =  TT_DESCENT,
        usWinAscent    =  TT_ASCENT,
        usWinDescent   = -TT_DESCENT,
        sCapHeight     = TT_PIXEL_SIZE * 14,  # Height of "H".
        sxHeight       = TT_PIXEL_SIZE * 10,  # Height of "x".
        # Magic numbers. Can these be computed?
        fsType              = 0,
        ySubscriptXSize     = 1434,
        ySubscriptYSize     = 1331,
        ySubscriptXOffset   = 0,
        ySubscriptYOffset   = 287,
        ySuperscriptXSize   = 1434,
        ySuperscriptYSize   = 1331,
        ySuperscriptXOffset = 0,
        ySuperscriptYOffset = 977,
        yStrikeoutSize      = 102,   # Suggested for 2048 unitsPerEm.
        yStrikeoutPosition  = 460,   # Suggested for 2048 unitsPerEm.
        fsSelection         = 0x0040 # REGULAR style.
        )
    fb.font["OS/2"].panose.bFamilyType = 2     # Latin Text
    fb.font["OS/2"].panose.bSerifStyle = 4     # Square Cove
    fb.font["OS/2"].panose.bWeight     = 5     # Book
    fb.font["OS/2"].panose.bProportion = 9     # Monospaced
    fb.font["OS/2"].recalcCodePageRanges(fb.font)
    fb.font["OS/2"].recalcUnicodeRanges(fb.font)
    fb.font["OS/2"].updateFirstAndLastCharIndex(fb.font)
    # Remove Katakana bit.
    # This makes the Windows font icon show "Abg" instead of Asian glyphs.
    fb.font["OS/2"].ulUnicodeRange2 &= ~(1 << 18)

def gasp_table():
    """
    @return  The gasp table.
    """
    gasp = table__g_a_s_p()
    gasp.version = 1
    gasp.gaspRange = {
        8:     0x0002, # Up to 8 ppem: grayscale only.
        16:    0x000A, # Up to 16 ppem: grid-fit and symmetric smoothing.
        65535: 0x000F  # Above 16 ppem: all flags enabled.
    }
    return gasp

def create_svg_files():
    """
    Create the svg/Uxxxx.svg files.
    """
    for codepoint, glyph in GLYPH.items():
        with open(f"svg/U{codepoint:04x}.svg", "w", encoding="utf-8") as file:
            file.write(create_svg(glyph["contour"], codepoint, scale=16))

def main():
    """
    Start the ball rolling.
    """
    create_true_type()
    create_svg_files()

################################################################################

FONT          = "Gallant Raster Term"
STYLE         = "Regular"
VERSION       = "1.000"
ASCENT        = 17
DESCENT       = 5
WIDTH         = 12
TT_PIXEL_SIZE = 64
TT_ASCENT     = TT_PIXEL_SIZE * ASCENT
TT_DESCENT    = TT_PIXEL_SIZE * -DESCENT
GLYPH         = read_src_file("gallant.src")
if __name__ == "__main__":
    main()

# vim: set ts=4:
