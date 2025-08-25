#!/bin/sh

while read -r name first last; do
  ./lscp "0x$first" "0x$last" > "$name.txt"
  ./txttopng -f gallant.hex -t "$name.txt" -p "Images/$name.png"
done << EOF
BasicLatin 0000 007F
Latin-1-Supplement 00A0 0100
Latin-Extended-A 0100 0180
Latin-Extended-B 0180 0250
IPA-Extensions 0250 02B0
Spacing-Modifier-Letters 02B0 0300
Combining-Diacritical-Marks 0300 0370
Greek-and-Coptic 0370 0400
Cyrillic 0400 0500
Latin-Extended-Additional 1E00 1F00
Greek-Extended 1F00 2000
General-Punctuation 2000 2070
Superscripts-and-Subscripts 2070 20A0
Currency-Symbols 20A0 20D0
Combining-Diacritical-Marks-for-Symbols 20D0 2100
Letterlike-Symbols 2100 2150
Number-Forms 2150 2190
Arrows 2190 2200
Mathematical-Operators 2200 2300
Miscellaneous-Technical 2300 2400
Control-Pictures 2400 2440
Optical-Character-Recognition 2440 2460
Enclosed-Alphanumerics 2460 2500
Box-Drawing 2500 2580
Block-Elements 2580 25A0
Geometric-Shapes 25A0 2600
Miscellaneous-Symbols 2600 2700
Dingbats 2700 27C0
Miscellaneous-Mathematical-Symbols-A 27C0 27F0
Supplemental-Arrows-A 27F0 2800
Braille-Patterns 2800 2900
Supplemental-Arrows-B 2900 2980
Supplemental-Mathematical-Operators 2A00 2B00
Misclellaneous-Symbols-and-Arrows 2B00 2C00
Katakana 30A0 3100
Private-Use-Area E0A0 E0F0
Alphabetic-Presentation-Forms FB00 FB50
Specials FFF0 10000
EOF

./txttopng -f gallant.hex -t "$HOME/UTF-8-demo.txt" -p "Images/UTF-8-demo-Markus-Kuhn-2002.png"
