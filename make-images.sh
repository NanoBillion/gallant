#!/bin/sh

while read -r first last name; do
  ./lscp "0x$first" "0x$last" > "$name.txt"
  ./txttopng -f gallant.hex -t "$name.txt" -p "Images/$first-$name.png"
  ./txttopng -f gallant.hex -t "$name.txt" -p "Images/$first-$name-Inverted.png" -i
done << EOF
0020 0080 Basic-Latin
00A0 0100 Latin-1-Supplement
0100 0180 Latin-Extended-A
0180 0250 Latin-Extended-B
0250 02B0 IPA-Extensions
02B0 0300 Spacing-Modifier-Letters
0300 0370 Combining-Diacritical-Marks
0370 0400 Greek-and-Coptic
0400 0500 Cyrillic
1E00 1F00 Latin-Extended-Additional
1F00 2000 Greek-Extended
2000 2070 General-Punctuation
2070 20A0 Superscripts-and-Subscripts
20A0 20D0 Currency-Symbols
20D0 2100 Combining-Diacritical-Marks-for-Symbols
2100 2150 Letterlike-Symbols
2150 2190 Number-Forms
2190 2200 Arrows
2200 2300 Mathematical-Operators
2300 2400 Miscellaneous-Technical
2400 2440 Control-Pictures
2440 2460 Optical-Character-Recognition
2460 2500 Enclosed-Alphanumerics
2500 2580 Box-Drawing
2580 25A0 Block-Elements
25A0 2600 Geometric-Shapes
2600 2700 Miscellaneous-Symbols
2700 27C0 Dingbats
27C0 27F0 Miscellaneous-Mathematical-Symbols-A
27F0 2800 Supplemental-Arrows-A
2800 2900 Braille-Patterns
2900 2980 Supplemental-Arrows-B
2A00 2B00 Supplemental-Mathematical-Operators
2B00 2C00 Miscellaneous-Symbols-and-Arrows
30A0 3100 Katakana
E0A0 E0F0 Private-Use-Area
FB00 FB50 Alphabetic-Presentation-Forms
FFF0 10000 Specials
EOF

./txttopng -f gallant.hex -t "UTF-8-demo.txt" -p "Images/UTF-8-demo-Markus-Kuhn-2002.png"
./txttopng -f gallant.hex -t "UTF-8-demo.txt" -p "Images/UTF-8-demo-Markus-Kuhn-2002-Inverted.png" -i
