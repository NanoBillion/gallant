#!/usr/bin/env python3
import sys

if sys.version_info[0] < 3:
    sys.stderr.write("Error: This script requires Python 3. Please run it with: python build_font.py\n")
    sys.exit(1)

from fontTools.fontBuilder import FontBuilder
from fontTools.pens.ttGlyphPen import TTGlyphPen
from fontTools.ttLib.tables._g_a_s_p import table__g_a_s_p

def main():
    print("Reading gallant.src...")
    glyphs_pixels = {}
    
    with open("gallant.src", "r", encoding="utf-8") as f:
        current_cp = None
        current_rows = []
        for line in f:
            line = line.strip()
            if line.startswith("STARTCHAR"):
                parts = line.split()
                if len(parts) >= 2:
                    cp_str = parts[1]
                    if cp_str.startswith("U"):
                        current_cp = int(cp_str[1:], 16)
                    else:
                        current_cp = int(cp_str, 16)
                current_rows = []
            elif line.startswith("ENDCHAR"):
                if current_cp is not None:
                    if len(current_rows) == 22:
                        current_rows.reverse()
                        glyphs_pixels[current_cp] = current_rows
                    else:
                        print("Warning: Glyph U+{:04X} has {} rows instead of 22".format(current_cp, len(current_rows)))
                current_cp = None
            elif "|" in line:
                first_pipe = line.find("|")
                last_pipe = line.rfind("|")
                if first_pipe != -1 and last_pipe != -1 and first_pipe < last_pipe:
                    content = line[first_pipe + 1 : last_pipe]
                    current_rows.append(content)

    print("Loaded {} glyph definitions from source.".format(len(glyphs_pixels)))

    # Design parameters
    pixel_size = 64
    units_per_em = 2048

    # Cell dimensions in pixels: 12x22 (or 24x22 double-width)
    # Descent is 5, Ascent is 17.
    # Therefore, baseline is at row 5 (0-indexed from bottom).
    cell_height_px = 22
    descent_px = 5
    ascent_px = 17

    ascent = ascent_px * pixel_size     # 1088
    descent = -descent_px * pixel_size  # -320
    
    # Initialize FontBuilder
    fb = FontBuilder(units_per_em, isTTF=True)

    # 1. Setup Glyph Order (Windows expects the first 4 glyphs to be .notdef, .null, nonmarkingreturn, and space)
    glyph_order = [".notdef", ".null", "nonmarkingreturn", "space"]
    cmap = {
        0x0000: ".null",
        0x000D: "nonmarkingreturn",
        0x0020: "space"
    }

    # Add remaining codepoints to order and cmap
    # Note: We explicitly skip U+FFFE and U+FFFF because they are reserved Unicode non-characters.
    # Windows GDI will flag the font as invalid if they are mapped in the cmap table.
    for cp in sorted(glyphs_pixels.keys()):
        if cp in (0x0000, 0x000D, 0x0020, 0xFFFE, 0xFFFF):
            continue
        name = "uni{:04X}".format(cp) if cp <= 0xFFFF else "u{:05X}".format(cp)
        glyph_order.append(name)
        cmap[cp] = name
    
    fb.setupGlyphOrder(glyph_order)

    # 2. Build Glyphs
    glyphs = {}
    metrics = {}

    # Helper function to render pixel lines into TTF outlines using boundary-tracing
    def draw_glyph_outline(rows, width_px):
        active_pixels = set()
        for r in range(cell_height_px):
            row_str = rows[r]
            for c in range(width_px):
                if row_str[c] == "█":
                    active_pixels.add((r, c))

        # Collect directed boundary edges (clockwise for active pixel blocks)
        edges = []
        for r, c in active_pixels:
            y_c = r - descent_px
            # Bottom edge: (c, y_c) -> (c + 1, y_c)
            if (r - 1, c) not in active_pixels:
                edges.append(((c, y_c), (c + 1, y_c)))
            # Right edge: (c + 1, y_c) -> (c + 1, y_c + 1)
            if (r, c + 1) not in active_pixels:
                edges.append(((c + 1, y_c), (c + 1, y_c + 1)))
            # Top edge: (c + 1, y_c + 1) -> (c, y_c + 1)
            if (r + 1, c) not in active_pixels:
                edges.append(((c + 1, y_c + 1), (c, y_c + 1)))
            # Left edge: (c, y_c + 1) -> (c, y_c)
            if (r, c - 1) not in active_pixels:
                edges.append(((c, y_c + 1), (c, y_c)))

        # Chain edges into loops
        adj = {}
        for start, end in edges:
            adj.setdefault(start, []).append(end)

        loops = []
        while adj:
            start_v = next(iter(adj.keys()))
            curr_v = start_v
            loop = [curr_v]
            while True:
                if curr_v not in adj:
                    break
                next_options = adj[curr_v]
                next_v = next_options.pop()
                if not next_options:
                    del adj[curr_v]
                if next_v == start_v:
                    break
                loop.append(next_v)
                curr_v = next_v
            loops.append(loop)

        # Simplify loops and scale coordinates
        pen = TTGlyphPen(None)
        for loop in loops:
            # Simplify collinear points
            simplified = []
            n = len(loop)
            if n >= 3:
                for i in range(n):
                    prev_v = loop[i - 1]
                    curr_v = loop[i]
                    next_v = loop[(i + 1) % n]
                    # Check if collinear on grid axes
                    if (prev_v[0] == curr_v[0] == next_v[0]) or (prev_v[1] == curr_v[1] == next_v[1]):
                        continue
                    simplified.append(curr_v)
            else:
                simplified = loop

            if not simplified:
                continue

            # Scale to font units and draw
            start_pt = (simplified[0][0] * pixel_size, simplified[0][1] * pixel_size)
            pen.moveTo(start_pt)
            for pt in simplified[1:]:
                scaled_pt = (pt[0] * pixel_size, pt[1] * pixel_size)
                pen.lineTo(scaled_pt)
            pen.closePath()

        return pen.glyph()

    # Define .notdef glyph using U+FFFD outline if available, otherwise fallback to hollow box
    if 0xFFFD in glyphs_pixels:
        glyphs[".notdef"] = draw_glyph_outline(glyphs_pixels[0xFFFD], len(glyphs_pixels[0xFFFD][0]))
        # Find left side bearing for .notdef
        rows = glyphs_pixels[0xFFFD]
        width_px = len(rows[0])
        left_side_bearing = width_px
        for r in rows:
            lsb = width_px - len(r.lstrip(' '))
            if lsb < left_side_bearing:
                left_side_bearing = lsb
        if left_side_bearing == width_px:
            left_side_bearing = 0
        metrics[".notdef"] = (width_px * pixel_size, left_side_bearing * pixel_size)
    else:
        # Define .notdef glyph: a simple hollow rectangle
        pen = TTGlyphPen(None)
        # Outer box (clockwise)
        pen.moveTo((0, -320))
        pen.lineTo((0, 1088))
        pen.lineTo((768, 1088))
        pen.lineTo((768, -320))
        pen.closePath()
        # Inner box (counter-clockwise to carve it out)
        pen.moveTo((64, -256))
        pen.lineTo((704, -256))
        pen.lineTo((704, 1024))
        pen.lineTo((64, 1024))
        pen.closePath()
        glyphs[".notdef"] = pen.glyph()
        metrics[".notdef"] = (768, 0)


    # Draw the special first 4 glyphs (forcing .null and nonmarkingreturn to be empty)
    # .null (U+0000) - must be empty
    glyphs[".null"] = TTGlyphPen(None).glyph()
    metrics[".null"] = (768, 0)

    # nonmarkingreturn (U+000D) - must be empty
    glyphs["nonmarkingreturn"] = TTGlyphPen(None).glyph()
    metrics["nonmarkingreturn"] = (768, 0)

    # space (U+0020) - must be empty
    glyphs["space"] = TTGlyphPen(None).glyph()
    metrics["space"] = (768, 0)

    # Draw remaining glyphs
    for cp, rows in glyphs_pixels.items():
        if cp in (0x0000, 0x000D, 0x0020, 0xFFFE, 0xFFFF):
            continue
        name = cmap[cp]
        width_px = len(rows[0])
        advance_width = width_px * pixel_size
        left_side_bearing = width_px
        for r in rows:
            lsb = width_px - len(r.lstrip(' '))
            if lsb < left_side_bearing:
                left_side_bearing = lsb
        if left_side_bearing == width_px:
            left_side_bearing = 0
        metrics[name] = (advance_width, left_side_bearing * pixel_size)
        glyphs[name] = draw_glyph_outline(rows, width_px)
    

    print("Setting up glyf table...")
    fb.setupGlyf(glyphs)

    # 3. Setup Character Map
    print("Setting up cmap table...")
    fb.setupCharacterMap(cmap)

    # 4. Setup Horizontal Metrics
    print("Setting up hmtx table...")
    fb.setupHorizontalMetrics(metrics)

    # 5. Setup Horizontal Header (hhea)
    print("Setting up hhea table...")
    fb.setupHorizontalHeader(ascent=ascent, descent=descent)

    # 6. Setup Name Table (using Gallant Mono Win to avoid caching conflicts)
    print("Setting up name table...")
    nameStrings = {
        "familyName": dict(en="Gallant Mono Win"),
        "styleName": dict(en="Regular"),
        "uniqueFontIdentifier": dict(en="Gallant Mono Win Regular; 1.000"),
        "fullName": dict(en="Gallant Mono Win Regular"),
        "psName": dict(en="GallantMonoWin-Regular"),
        "version": dict(en="Version 1.000"),
    }
    fb.setupNameTable(nameStrings)

    # 7. Setup OS/2 table
    print("Setting up OS/2 table...")
    fb.setupOS2(
        sTypoAscender=ascent,
        sTypoDescender=descent,
        sTypoLineGap=0,
        usWinAscent=ascent,
        usWinDescent=-descent,
        usWeightClass=400,
        usWidthClass=5,
        fsType=0
    )

    # Populate subscript, superscript, and strikeout metrics (required by Windows GDI)
    print("Populating subscript, superscript, and strikeout sizes...")
    os2 = fb.font["OS/2"]
    os2.ySubscriptXSize = 1434
    os2.ySubscriptYSize = 1331
    os2.ySubscriptXOffset = 0
    os2.ySubscriptYOffset = 287
    os2.ySuperscriptXSize = 1434
    os2.ySuperscriptYSize = 1331
    os2.ySuperscriptXOffset = 0
    os2.ySuperscriptYOffset = 977
    os2.yStrikeoutSize = 102
    os2.yStrikeoutPosition = 512
    os2.sCapHeight = 1088
    os2.sxHeight = 704

    # 8. Setup PostScript (post) Table (Format 3.0 to omit glyph names)
    print("Setting up post table...")
    fb.setupPost(keepGlyphNames=False)

    # 9. Setup Dummy DSIG table (required by Windows)
    print("Setting up dummy DSIG table...")
    fb.setupDummyDSIG()

    # 10. Setup gasp table for Windows rendering
    print("Setting up gasp table...")
    gasp = table__g_a_s_p()
    gasp.version = 1
    gasp.gaspRange = {
        8: 0x0002,   # Up to 8 ppem: grayscale only
        16: 0x000A,  # Up to 16 ppem: grid-fit and symmetric smoothing
        65535: 0x000F # Above 16 ppem: all flags enabled
    }
    fb.font["gasp"] = gasp

    # 11. Apply Monospace and Regular style flags
    print("Applying Monospace and style classifications...")
    # OS/2 PANOSE proportion code 9 = Monospaced
    fb.font["OS/2"].panose.bFamilyType = 2
    fb.font["OS/2"].panose.bSerifStyle = 11
    fb.font["OS/2"].panose.bWeight = 5
    fb.font["OS/2"].panose.bProportion = 9
    
    # OS/2 fsSelection code 64 (0x0040) = REGULAR style
    fb.font["OS/2"].fsSelection = 0x0040
    
    # post table isFixedPitch = 1 (True)
    fb.font["post"].isFixedPitch = 1

    # 12. Recalculate OS/2 metrics, code pages, and Unicode ranges
    print("Recalculating OS/2 ranges and character indices...")
    fb.font["OS/2"].recalcCodePageRanges(fb.font)
    fb.font["OS/2"].recalcUnicodeRanges(fb.font)
    fb.font["OS/2"].updateFirstAndLastCharIndex(fb.font)

    # Save TTF
    ttf_filename = "gallant.ttf"
    print("Saving {}...".format(ttf_filename))
    fb.save(ttf_filename)

    # Save WOFF
    print("Saving WOFF flavor...")
    fb.font.flavor = "woff"
    fb.save("gallant.woff")

    # Save WOFF2
    print("Saving WOFF2 flavor...")
    fb.font.flavor = "woff2"
    fb.save("gallant.woff2")

    print("Font build complete! Generated:")
    print(" - gallant.ttf")
    print(" - gallant.woff")
    print(" - gallant.woff2")

    # 13. Perform self-test on Windows
    if sys.platform == "win32":
        print("\nPerforming Windows GDI load test...")
        import ctypes
        import os
        font_path = os.path.abspath(ttf_filename)
        flags = 0x10 | 0x20
        res = ctypes.windll.gdi32.AddFontResourceExW(font_path, flags, 0)
        if res > 0:
            ctypes.windll.gdi32.RemoveFontResourceExW(font_path, flags, 0)
            print("Windows GDI Load Test: PASSED! (The font is fully valid on Windows.)")
        else:
            print("Windows GDI Load Test: FAILED. (Windows rejects this font.)")

if __name__ == "__main__":
    main()
