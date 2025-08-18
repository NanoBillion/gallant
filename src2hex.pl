#!/usr/bin/env perl
#

use warnings;
use diagnostics;
use strict;
use utf8;
binmode(STDIN,  ":encoding(UTF-8)");
binmode(STDOUT, ":encoding(UTF-8)");
binmode(STDERR, ":encoding(UTF-8)");

my $width = 12;
my $height = 22;
my $dblwidth = 2 * $width;

print "# Width: $width\n";
print "# Height: $height\n";
while (<>) {
  if (/^STARTCHAR U\+([[:xdigit:]]{4,})/) {
    print "$1:";
  }
  elsif (/^[[:digit:]]+ \|([ █]{$width})\|/u) {
    my $bits = $1;
    $bits =~ tr/ █/01/;
    print unpack("H*", pack("B*", $bits . "0000")); # 12 bits + 0000
  }
  elsif (/^[[:digit:]]+ \|([ █]{$dblwidth})\|/u) {
    my $bits = $1;
    $bits =~ tr/ █/01/;
    print unpack("H*", pack("B*", $bits)); # 24 bits
  }
  elsif (/^ENDCHAR$/) {
    print "\n";
  }
  else {
    print STDERR "unrecognized line: $_";
    exit 1;
  }
}

# vi: set tabstop=2 shiftwidth=2 expandtab fileformat=unix:
