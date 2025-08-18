#!/usr/bin/env perl

use v5.36;
use warnings;
use diagnostics;
use strict;
use utf8;
use open qw(:std :encoding(UTF-8));
use charnames ':full';

my $width  = 12;
my $height = 22;
my $dblwidth = 2 * $width;

my $lnr = 0;
my $n   = $height;
while (<>) {
  chop;
  ++$lnr;
  if (/^STARTCHAR\s+[Uu]\+([[:xdigit:]]+)/) {
    my $cp   = hex $1;
    my $name = charnames::viacode ($cp) // '<no name>';
    $_ = sprintf "STARTCHAR U+%04x %s", $cp, $name;
  }
  elsif (/^[[:digit:]]+ \|(.{$width})\|/) {
    $_ = sprintf "%02d |%s|", $n--, $1;
  }
  elsif (/^[[:digit:]]+ \|(.{$dblwidth})\|/) {
    $_ = sprintf "%02d |%s|", $n--, $1;
  }
  elsif (/^ENDCHAR$/) {
    if ($n != 0) {
      my $N = 22 - $n;
      print STDERR
        "line $lnr: ENDCHAR after $N bitmap rows, expected $height\n";
      exit 1;
    }
    $n = $height;
  }
  else {
    print STDERR "line $lnr: unrecognized input: '$_'\n";
    exit 1;
  }
  print "$_\n";
}

# vi: set tabstop=2 shiftwidth=2 expandtab fileformat=unix:
