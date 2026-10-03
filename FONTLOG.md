# FONTLOG for Gallant Raster Term

This file records the changes to the glyphs.

In the table below, the *Version* column is of the form `#.###`,
starting with 1.000 and increased whenever a change to `gallant.src` is
made. This version string is embedded into the TrueType font's "name"
table members `version` with a value of `Version 1.000` and
`uniqueFontIdentifier` with a value of `Gallant Raster Term Regular;
1.000`. The table is sorted by version, latest first.

The `Glyphs` column is the number of glyphs as determined with `grep -c
STARTCHAR gallant.src`


|Version |Glyphs | Change                                         |   Date     |
|--------|-------|------------------------------------------------|------------|
|1.006   |5114   | Add Latin-Extended-E as of Unicode 17.0.       | 2026-10-03 |
|1.005   |5054   | Fix two missing pixels in U+29d1.              | 2026-10-01 |
|1.004   |5054   | Add Phonetic Extensions and Supplement.        | 2026-09-30 |
|1.003   |4862   | Complete Miscellaneous Symbols.                | 2026-09-25 |
|1.002   |4702   | Complete Miscellaneous Symbols and Arrows.     | 2026-09-17 |
|1.001   |4671   | Complete Latin Extended-C.                     | 2026-09-16 |
|1.000   |4639   | New way of creating `gallant.ttf`.             | 2026-09-03 |

