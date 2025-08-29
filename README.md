# Gallant Font

This is the Gallant font, as used by the Sun Microsystems SPARCstation
console, extended with glyphs for many Unicode blocks. It is a raster
font with a 22x12 pixel character cell, descent 5 and ascent 17.

The starting point was the `gallant.hex` file as found on FreeBSD 14,
which contained 502 glyphs at the time. This project currently provides
more than 4500 glyphs. Major additions:

* Greek
* Cyrillic
* International Phonetic Association Extensions
* Extended Latin characters
* Zapf Dingbats
* Tons of arrows
* Tons of mathematical symbols
* Letterlike symbols and enclosed alphanumerics
* Pixel-perfect box drawing
* Currency symbols
* More punctuation
* Just enough Katakana to say コンニチハ
* Powerline glyphs in the *Private Use Area* at U+e0a0

I have tried hard to keep the look of the font for Greek and Cyrillic
glyphs, but not for symbols like arrows and mathematical operators where
it makes no sense. Please let me know where I messed up (I can only
barely read and write Greek, and hardly any Cyrillic and no Japanese
at all). I used the
documents of *The Unicode Standard, Version 16.0* as guidance.
See [Unicode.org](https://home.unicode.org/) for more on Unicode.

## Unicode Support

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
|U+0300 - U+036F|[Combining Diacritical Marks](https://www.unicode.org/charts/PDF/U0300.pdf)            |Complete |
|U+0370 - U+03FF|[Greek and Coptic](https://www.unicode.org/charts/PDF/U0370.pdf)                       |Complete |
|U+0400 - U+04FF|[Cyrillic](https://www.unicode.org/charts/PDF/U0400.pdf)                               |Complete |
|U+0500 - U+052F|[Cyrillic Supplement](https://www.unicode.org/charts/PDF/U0500.pdf)                    |TODO     |
|U+0530 - U+1CFF|[...Many foreign alphabets...](https://www.unicode.org/charts/PDF/U0530.pdf)           |TODO     |
|U+1D00 - U+1D7F|[Phonetic Extensions](https://www.unicode.org/charts/PDF/U1D00.pdf)                    |TODO     |
|U+1D80 - U+1DBF|[Phonetic Extensions Supplement](https://www.unicode.org/charts/PDF/U1D80.pdf)         |TODO     |
|U+1DC0 - U+1DFF|[Combining Diacritical Marks Supplement](https://www.unicode.org/charts/PDF/U1DC0.pdf) |TODO     |
|U+1E00 - U+1EFF|[Latin Extended Additional](https://www.unicode.org/charts/PDF/U1E00.pdf)              |Complete |
|U+1F00 - U+1FFF|[Greek Extended](https://www.unicode.org/charts/PDF/U1F00.pdf)                         |Complete |
|U+2000 - U+206F|[General Punctuation](https://www.unicode.org/charts/PDF/U2000.pdf)                    |Complete |
|U+2070 - U+209F|[Superscripts and Subscripts](https://www.unicode.org/charts/PDF/U2070.pdf)            |Complete |
|U+20A0 - U+20CF|[Currency Symbols](https://www.unicode.org/charts/PDF/U20A0.pdf)                       |Complete |
|U+20D0 - U+20FF|[Combining Diacritical Marks for Symbols](https://www.unicode.org/charts/PDF/U20D0.pdf)|Complete |
|U+2100 - U+214F|[Letterlike Symbols](https://www.unicode.org/charts/PDF/U2100.pdf)                     |Complete |
|U+2150 - U+218F|[Number Forms](https://www.unicode.org/charts/PDF/U2150.pdf)                           |Complete |
|U+2190 - U+21FF|[Arrows](https://www.unicode.org/charts/PDF/U2190.pdf)                                 |Complete |
|U+2200 - U+22FF|[Mathematical Operators](https://www.unicode.org/charts/PDF/U2200.pdf)                 |Complete |
|U+2300 - U+23FF|[Miscellaneous Technical](https://www.unicode.org/charts/PDF/U2300.pdf)                |Complete |
|U+2400 - U+243F|[Control Pictures](https://www.unicode.org/charts/PDF/U2400.pdf)                       |Complete |
|U+2440 - U+245F|[Optical Character Recognition](https://www.unicode.org/charts/PDF/U2440.pdf)          |Complete |
|U+2460 - U+24FF|[Enclosed Alphanumerics](https://www.unicode.org/charts/PDF/U2460.pdf)                 |Complete |
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
|U+2C80 - U+30BF|[...Many foreign alphabets...](https://www.unicode.org/charts/PDF/U2C80.pdf)           |TODO     |
|U+30A0 - U+30FF|[Katakana](https://www.unicode.org/charts/PDF/U30A0.pdf)                               |Partial  |
|U+3100 - U+A6FF|[...Many foreign alphabets...](https://www.unicode.org/charts/PDF/U2C80.pdf)           |TODO     |
|U+A700 - U+A71F|[Modifier Tone Letters](https://www.unicode.org/charts/PDF/UA700.pdf)                  |TODO     |
|U+A720 - U+A7FF|[Latin Extended-D](https://www.unicode.org/charts/PDF/UA720.pdf)                       |TODO     |
|U+A800 - U+AB2F|[...Many foreign alphabets...](https://www.unicode.org/charts/PDF/UA800.pdf)           |TODO     |
|U+AB30 - U+AB6F|[Latin Extended-E](https://www.unicode.org/charts/PDF/UAB30.pdf)                       |TODO     |
|U+AB70 - U+FAFF|[...Many foreign alphabets...](https://www.unicode.org/charts/PDF/UAB70.pdf)           |TODO     |
|U+FB00 - U+FB4F|[Alphabetic Presentation Forms](https://www.unicode.org/charts/PDF/UFB00.pdf)          |Partial  |
|U+FB50 - U+FFEF|[...Many foreign alphabets...](https://www.unicode.org/charts/PDF/UFB50.pdf)           |TODO     |
|U+FFF0 - U+FFFF|[Specials](https://www.unicode.org/charts/PDF/UFFF0.pdf)                               |Complete |

Codepoints >= U+10000 can be part of a BDF file, but cannot be used by
core X11 since it internally restricts glyph encoding numbers to 16 bit.
(This limitation does not apply to **Xft** rendered fonts).

The FreeBSD
[vt(4)](https://man.freebsd.org/cgi/man.cgi?query=vt&apropos=0&sektion=4)
driver works fine with the full 32 bit codepoint range.

## How does it look?

The [Images](Images) directory contains white-on-black and
black-on-white (suffixed `-Inverted`) PNG image files for the separate
blocks and [Markus Kuhn's](https://www.cl.cam.ac.uk/~mgk25/)
[UTF-8-demo.txt](https://www.cl.cam.ac.uk/~mgk25/ucs/examples/UTF-8-demo.txt),
licensed [CC BY](https://creativecommons.org/licenses/by/4.0/); no changes
were made to the text before rendering it in Gallant.

## How do I load/use this font?

### As an X11 Raster Font, e.g. for Xterm(1)

Install the BDF file `gallant.bdf` where X11 looks for fonts. You can
query the current font path with `xset q | sed -n '/^Font/,/^ /p'`. If
you do not have write permission to any of the font directories, you may
create your own under, say, `$HOME/.fonts`.

The following example uses `$HOME/.fonts` as the font directory and adds
it to the font path. To make the font path addition permanent, you
should add the `xset` lines to your `$HOME/.xinitrc` or equivalent X11
startup file (common candidates are `.xsession` and `.xprofile`).

```
mkdir -p $HOME/.fonts
cp gallant.bfd $HOME/.fonts
cd $HOME/.fonts
mkfontdir
xset fp+ $HOME/.fonts
xset fp rehash
xterm -fa '' -fn "-sun-gallant-medium-r-normal-*-22-*-*-*-*-80-*-*"
```

### FreeBSD Console

```
vidcontrol -f /path/to/gallant.fnt
```

You may also drop the font files into directory `/usr/share/vt/fonts` and
add `allscreens_flags="-f gallant"` to your `/etc/rc.conf`. This way all
console terminals use the font after boot.

### Linux, NetBSD, OpenBSD Console

The Linux console uses
[PSF](https://en.wikipedia.org/wiki/PC_Screen_Font) fonts. As of 2025
this format can only contain 256 or 512 glyphs.

The NetBSD console is also restricted to 512 glyphs. It does not handle
double width or combining characters.

OpenBSD inherited the `wscons` from NetBSD, so similar restrictions apply.

The glyphs in this gallant project would have to be severely reduced in
number to fit. If someone wants to contribute a stripped down font in
the appropriate format, I'm willing to add it to this project.

## Who are you?

I'm an ex-Sun Microsystems software engineer who had a stint in the
company shortly before Oracle took over (2008/2009). I was nowhere near
the OpenBoot PROM files which contained the gallant font. My first
contact with SUN hardware was in the late 80's and early 90's at
university with the 3/60 and the SPARCstations. It was then and there
that the gallant font and the Trinitron CRT raster were burnt in my
retina.

## History

The oldest reference to the Gallant font I could find was in a Copyright
notice in [NetBSD's
gallant12x22.h](https://ftp.netbsd.org/pub/NetBSD/NetBSD-current/src/sys/dev/wsfont/gallant12x22.h)
which reads:

```c
/*
 * Copyright (c) 1992, 1993
 *     The Regents of the University of California.  All rights reserved.
 *
 * This code is derived from software contributed to the Computer Systems
 * Engineering Group at Lawrence Berkeley Laboratory and to the University
 * of California at Berkeley by Jef Poskanzer.
 * [...]
 * Derived from: @(#)gallant19.h	8.1 (Berkeley) 6/11/93
 */
```

In private conversation with the author, Jef said he guessed that the
gallant font was designed by someone at Sun Microsystems before it made
its way to Berkeley.

## Remarks

Codepoints 0 through 31 contain glyphs of the VT100 line-drawing
character set othwise known as the [DEC Special Character and Line
Drawing Set](https://en.wikipedia.org/wiki/DEC_Special_Graphics). They
were left unmodified to not change their intended use by applications
that expect them there. Each of the special characters also has an
"official" Unicode codepoint. Contemporary applications should never
need to render them. This is the mapping:

|Special |Official |Name |
|--------|---------|---------|
|U+0000  |U+25AE   |black vertical rectangle |
|U+0001  |U+25C6   |black diamond |
|U+0002  |U+2592   |medium shade |
|U+0003  |U+2409   |symbol for horizontal tabulation |
|U+0004  |U+240C   |symbol for form feed |
|U+0005  |U+240D   |symbol for carriage return |
|U+0006  |U+240A   |symbol for line feed |
|U+0007  |U+00B0   |degree sign |
|U+0008  |U+00B1   |plus-minus sign |
|U+0009  |U+2424   |symbol for newline |
|U+000a  |U+240B   |symbol for vertical tabulation |
|U+000b  |U+2518   |box drawings light up and left |
|U+000c  |U+2510   |box drawings light down and left |
|U+000d  |U+250C   |box drawings light down and right |
|U+000e  |U+2514   |box drawings light up and right |
|U+000f  |U+253C   |box drawings light vertical and horizontal |
|U+0010  |U+23BA   |box drawings scan 1 |
|U+0011  |U+23BB   |box drawings scan 3 |
|U+0012  |U+2500   |box drawings light horizontal |
|U+0013  |U+23BC   |box drawings scan 7 |
|U+0014  |U+23BD   |box drawings scan 9 |
|U+0015  |U+251C   |box drawings light vertical and right |
|U+0016  |U+2524   |box drawings light vertical and left |
|U+0017  |U+2534   |box drawings light up and horizontal |
|U+0018  |U+252C   |box drawings light down and horizontal |
|U+0019  |U+2502   |box drawings light vertical |
|U+001a  |U+2264   |less-than or equal to |
|U+001b  |U+2265   |greater-than or equal to |
|U+001c  |U+03C0   |greek small letter pi |
|U+001d  |U+2260   |not equal to |
|U+001e  |U+00A3   |pound sign |
|U+001f  |U+00B7   |middle dot |

## TODO

* Describe how to contribute
* Describe the helper programs
