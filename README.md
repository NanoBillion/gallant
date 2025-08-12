# Gallant Font

This is the Gallant font, as used by the Sun Microsystems SPARCstation
console, extended with glyphs for many Unicode blocks. It is a raster
font with a 22x12 pixel character cell, descent 5 and ascent 17.

The starting point was the `gallant.hex` file as found on FreeBSD 14,
which contained 502 glyphs at the time. This project currently provides
more than 3500 glyphs. Major additions:

* Greek
* Cyrillic
* International Phonetic Association Extensions
* Zapf Dingbats
* Tons of arrows
* Tons of mathematical symbols
* Pixel-perfect box drawing
* Currency symbols
* More punctuation
* Powerline glyphs in the *Private Use Area* at U+e0a0

I have tried hard to keep the look of the font for Greek and Cyrillic
glyphs, but not for symbols like arrows and mathematical operators where
it makes no sense. Please let me know where I messed up (I can only
barely read and write Greek, and hardly any Cyrillic). I used the
documents of *The Unicode Standard, Version 16.0* as guidance.
See [Unicode.org](https://home.unicode.org/) for more on Unicode.

## Unicode Support

Gallant currently contains only glyphs for blocks in the *Basic
Multilingual Plane* (Plane 0).

In the following table's Status column, *Complete* means all glyphs are
present, *Partial* means at least one glyph is present, *TODO* means
none are present.

|Block          |Name                                                                                   |Status   |
|---------------|---------------------------------------------------------------------------------------|---------|
|U+0000 - U+007F|[Basic Latin](https://www.unicode.org/charts/PDF/U0000.pdf)                            |Complete |
|U+0080 - U+00FF|[Latin-1 Supplement](https://www.unicode.org/charts/PDF/U0080.pdf)                     |Complete |
|U+0100 - U+017F|[Latin Extended-A](https://www.unicode.org/charts/PDF/U0100.pdf)                       |Complete |
|U+0180 - U+024F|[Latin Extended-B](https://www.unicode.org/charts/PDF/U0180.pdf)                       |Complete |
|U+0250 - U+02AF|[IPA Extensions](https://www.unicode.org/charts/PDF/U0250.pdf)                         |Complete |
|U+02B0 - U+02FF|[Spacing Modifier Letters](https://www.unicode.org/charts/PDF/U02B0.pdf)               |Complete |
|U+0300 - U+036F|[Combining Diacritical Marks](https://www.unicode.org/charts/PDF/U0300.pdf)            |Partial  |
|U+0370 - U+03FF|[Greek and Coptic](https://www.unicode.org/charts/PDF/U0370.pdf)                       |Partial  |
|U+0400 - U+04FF|[Cyrillic](https://www.unicode.org/charts/PDF/U0400.pdf)                               |Complete |
|U+0500 - U+052F|[Cyrillic Supplement](https://www.unicode.org/charts/PDF/U0500.pdf)                    |TODO     |
|U+0530 - U+1cff|[...Many foreign alphabets...](https://www.unicode.org/charts/PDF/U0530.pdf)           |TODO     |
|U+1D00 - U+1D7F|[Phonetic Extensions](https://www.unicode.org/charts/PDF/U1D00.pdf)                    |TODO     |
|U+1D80 - U+1DBF|[Phonetic Extensions Supplement](https://www.unicode.org/charts/PDF/U1D80.pdf)         |TODO     |
|U+1DC0 - U+1DFF|[Combining Diacritical Marks Supplement](https://www.unicode.org/charts/PDF/U1DC0.pdf) |TODO     |
|U+1E00 - U+1EFF|[Latin Extended Additional](https://www.unicode.org/charts/PDF/U1E00.pdf)              |TODO     |
|U+1F00 - U+1FFF|[Greek Extended](https://www.unicode.org/charts/PDF/U1F00.pdf)                         |Partial  |
|U+2000 - U+206F|[General Punctuation](https://www.unicode.org/charts/PDF/U2000.pdf)                    |Complete |
|U+2070 - U+209F|[Superscripts and Subscripts](https://www.unicode.org/charts/PDF/U2070.pdf)            |Complete |
|U+20A0 - U+20CF|[Currency Symbols](https://www.unicode.org/charts/PDF/U20A0.pdf)                       |Complete |
|U+20D0 - U+20FF|[Combining Diacritical Marks for Symbols](https://www.unicode.org/charts/PDF/U20D0.pdf)|Partial  |
|U+2100 - U+214F|[Letterlike Symbols](https://www.unicode.org/charts/PDF/U2100.pdf)                     |Partial  |
|U+2150 - U+218F|[Number Forms](https://www.unicode.org/charts/PDF/U2150.pdf)                           |TODO     |
|U+2190 - U+21FF|[Arrows](https://www.unicode.org/charts/PDF/U2190.pdf)                                 |Complete |
|U+2200 - U+22FF|[Mathematical Operators](https://www.unicode.org/charts/PDF/U2200.pdf)                 |Complete |
|U+2300 - U+23FF|[Miscellaneous Technical](https://www.unicode.org/charts/PDF/U2300.pdf)                |Complete |
|U+2400 - U+243F|[Control Pictures](https://www.unicode.org/charts/PDF/U2400.pdf)                       |TODO     |
|U+2440 - U+245F|[Optical Character Recognition](https://www.unicode.org/charts/PDF/U2440.pdf)          |Cmplete  |
|U+2460 - U+24FF|[Enclosed Alphanumerics](https://www.unicode.org/charts/PDF/U2460.pdf)                 |TODO     |
|U+2500 - U+257F|[Box Drawing](https://www.unicode.org/charts/PDF/U2500.pdf)                            |Complete |
|U+2580 - U+259F|[Block Elements](https://www.unicode.org/charts/PDF/U2580.pdf)                         |Complete |
|U+25A0 - U+25FF|[Geometric Shapes](https://www.unicode.org/charts/PDF/U25A0.pdf)                       |Complete |
|U+2600 - U+26FF|[Miscellaneous Symbols](https://www.unicode.org/charts/PDF/U2600.pdf)                  |Partial  |
|U+2700 - U+27BF|[Dingbats](https://www.unicode.org/charts/PDF/U2700.pdf)                               |Complete |
|U+27C0 - U+27EF|[Miscellaneous Mathematical Symbols-A](https://www.unicode.org/charts/PDF/U27C0.pdf)   |Complete |
|U+27F0 - U+27FF|[Supplemental Arrows-A](https://www.unicode.org/charts/PDF/U27F0.pdf)                  |Complete |
|U+2800 - U+28FF|[Braille Patterns](https://www.unicode.org/charts/PDF/U2800.pdf)                       |Complete |
|U+2900 - U+297F|[Supplemental Arrows-B](https://www.unicode.org/charts/PDF/U2900.pdf)                  |Complete |
|U+2980 - U+29FF|[Miscellaneous Mathematical Symbols-A](https://www.unicode.org/charts/PDF/U2980.pdf)   |Complete |
|U+2A00 - U+2AFF|[Supplemental Mathematical Operators](https://www.unicode.org/charts/PDF/U2A00.pdf)    |TODO     |
|U+2B00 - U+2BFF|[Miscellaneous Symbols and Arrows](https://www.unicode.org/charts/PDF/U2B00.pdf)       |Partial  |
|U+2C00 - U+2C5F|[Glagolitic](https://www.unicode.org/charts/PDF/U2C00.pdf)                             |TODO     |
|U+2C60 - U+2C7F|[Latin Extended-C](https://www.unicode.org/charts/PDF/U2C60.pdf)                       |TODO     |
|U+2C80 - U+A6FF|[...Many foreign alphabets...](https://www.unicode.org/charts/PDF/U2C80.pdf)           |TODO     |
|U+A700 - U+A71F|[Modifier Tone Letters](https://www.unicode.org/charts/PDF/UA700.pdf)                  |TODO     |
|U+A720 - U+A7FF|[Latin Extended-D](https://www.unicode.org/charts/PDF/UA720.pdf)                       |TODO     |
|U+A800 - U+AB2F|[...Many foreign alphabets...](https://www.unicode.org/charts/PDF/UA800.pdf)           |TODO     |
|U+AB30 - U+AB6F|[Latin Extended-E](https://www.unicode.org/charts/PDF/UAB30.pdf)                       |TODO     |
|U+AB70 - U+FAFF|[...Many foreign alphabets...](https://www.unicode.org/charts/PDF/UAB70.pdf)           |TODO     |
|U+FB00 - U+FB4F|[Alphabetic Presentation Forms](https://www.unicode.org/charts/PDF/UFB00.pdf)          |Partial  |
|U+FB50 - U+FFEF|[...Many foreign alphabets...](https://www.unicode.org/charts/PDF/UFB50.pdf)           |TODO     |
|U+FFF0 - U+FFFF|[Specials](https://www.unicode.org/charts/PDF/UFFF0.pdf)                               |Partial  |

## How do I load/use this font?

### FreeBSD

```
vidcontrol -f /path/to/gallant.fnt
```

You may also drop the font files into directory `/usr/share/vt/fonts` and
add `allscreens_flags="-f gallant"` to your `/etc/rc.conf`. This way all
console terminals use the font after boot.

## Remarks

* Codepoints 0 through 31 slightly differ from the Unicode standard.
  They were left unmodified to not change their intended use on SPARC hardware.

## TODO

* Describe how to contribute
* Describe the helper scripts
