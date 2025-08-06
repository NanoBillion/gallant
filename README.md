# Gallant Font

This is the Gallant font, as used by the Sun Microsystems SPARCstation
console, extended with glyphs for many Unicode blocks. It is a raster
font with a 22x12 pixel character cell, descent 5 and ascent 17.

The starting point was the `gallant.hex` file as found on FreeBSD 14,
which contained 502 glyphs at the time. This project currently provides
more than 3000 glyphs. Major additions:

* Greek
* Cyrillic
* International Phonetic Association Extensions
* Zapf Dingbats
* Tons of arrows
* Tons of mathematical symbols
* Pixel-perfect box drawing
* Currency symbols
* More punctuation

I have tried hard to keep the look of the font for Greek and Cyrillic
glyphs, but not for symbols like arrows and mathematical operators where
it makes no sense. Please let me know where I messed up.

## Unicode Support

Gallant currently contains only glyphs for blocks in the Basic
Multilingual Plane (Plane 0), which in its entirety could comprise 65535
glyphs. Status *Complete* means all glyphs are present, *Partial* means
at least one glyph is present, *TODO* means none are present.

|Block          |Name                                   |Status   |
|U+0000 - U+007F|Basic Latin                            |Complete |
|U+0080 - U+00FF|Latin-1 Supplement                     |Complete |
|U+0100 - U+017F|Latin Extended-A                       |Complete |
|U+0180 - U+024F|Latin Extended-B                       |Complete |
|U+0250 - U+02AF|IPA Extensions                         |Complete |
|U+02B0 - U+02FF|Spacing Modifier Letters               |Complete |
|U+0300 - U+036F|Combining Diacritical Marks            |TODO     |
|U+0370 - U+03FF|Greek and Coptic                       |Partial  |
|U+0400 - U+04FF|Cyrillic                               |Complete |
|U+0500 - U+052F|Cyrillic Supplement                    |TODO     |
|U+0530 - U+1cff|...Many foreign alphabets...           |TODO     |
|U+1D00 - U+1D7F|Phonetic Extensions                    |TODO     |
|U+1D80 - U+1DBF|Phonetic Extensions Supplement         |TODO     |
|U+1DC0 - U+1DFF|Combining Diacritical Marks Supplement |TODO     |
|U+1E00 - U+1EFF|Latin Extended Additional              |TODO     |
|U+1F00 - U+1FFF|Greek Extended                         |Partial  |
|U+2000 - U+206F|General Punctuation                    |Complete |
|U+2070 - U+209F|Superscripts and Subscripts            |Complete |
|U+20A0 - U+20CF|Currency Symbols                       |Complete |
|U+20D0 - U+20FF|Combining Diacritical Marks for Symbols|TODO     |
|U+2100 - U+214F|Letterlike Symbols                     |Partial  |
|U+2150 - U+218F|Number Forms                           |TODO     |
|U+2190 - U+21FF|Arrows                                 |Complete |
|U+2200 - U+22FF|Mathematical Operators                 |Complete |
|U+2300 - U+23FF|Miscellaneous Technical                |Complete |
|U+2400 - U+243F|Control Pictures                       |TODO     |
|U+2440 - U+245F|Optical Character Recognition          |Partial  |
|U+2460 - U+24FF|Enclosed Alphanumerics                 |TODO     |
|U+2500 - U+257F|Box Drawing                            |Complete |
|U+2580 - U+259F|Block Elements                         |Complete |
|U+25A0 - U+25FF|Geometric Shapes                       |Complete |
|U+2600 - U+26FF|Miscellaneous Symbols                  |Partial  |
|U+2700 - U+27BF|Dingbats                               |Complete |
|U+27C0 - U+27EF|Miscellaneous Mathematical Symbols-A   |Partial  |
|U+27F0 - U+27FF|Supplemental Arrows-A                  |Partial  |
|U+2800 - U+28FF|Braille Patterns                       |Complete |
|U+2900 - U+297F|Supplemental Arrows-B                  |Complete |
|U+2980 - U+29FF|Miscellaneous Mathematical Symbols-A   |Complete |
|U+2A00 - U+2AFF|Supplemental Mathematical Operatos     |TODO     |
|U+2B00 - U+2BFF|Miscellaneous Symbols and Arrows       |TODO     |
|U+2C00 - U+2C5F|Glagolitic                             |TODO     |
|U+2C60 - U+2C7F|Latin Extended-C                       |TODO     |
|U+2C80 - U+A6FF|...Many foreign alphabets...           |TODO     |
|U+A700 - U+A71F|Modifier Tone Letters                  |TODO     |
|U+A720 - U+A7FF|Latin Extended-D                       |TODO     |
|U+A800 - U+AB2F|...Many foreign alphabets...           |TODO     |
|U+AB30 - U+AB6F|Latin Extended-E                       |TODO     |
|U+AB70 - U+FAFF|...Many foreign alphabets...           |TODO     |
|U+FB00 - U+FB4F|Alphabetic Presentation Forms          |Partial  |
|U+FB50 - U+FFEF|...Many foreign alphabets...           |TODO     |
|U+FFF0 - U+FFFF|Specials                               |Partial  |

## How do I load/use this font?

### FreeBSD

  vidcontrol -f /path/to/gallant.fnt

You may also drop the font files into directory `/usr/share/vt/fonts` and
add `allscreens_flags="-f gallant"` to your `/etc/rc.conf`. This way all
console terminals use the font after boot.

## Remarks

* Codepoints 0 through 31 slightly differ from the Unicode standard.
  They were left unmodified to not change their intended use on SPARC hardware.

## TODO

* Describe how to contribute
* Describe the helper scripts
