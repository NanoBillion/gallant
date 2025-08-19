#!/usr/bin/env perl

use warnings;
use Getopt::Long;
use diagnostics;
use strict;

&initialization_and_options;
#      +-------------------+
#      |    Main Program   |
#      +-------------------+

my @bdf = ();
my $glyphs = 0;

my $fontfile = $ARGV[0];
open (my $fh, '<', $fontfile) or die "$::MYNAME: could not open $fontfile: $!";
while (my $line = <$fh>) {
  next if ($line =~ /^\s*#/);
  if ($line =~ /^([[:xdigit:]]+):([[:xdigit:]]+)$/) {
    if ($::opt_display) {
      &display_glyph($1, $2);
    }
    else {
      &convert_glyph($1, $2);
    }
  }
  else {
    print STDERR "$::MYNAME: ignoring unrecognized line: $line\n";
  }
}
close $fh;

unless ($::opt_display) {
  &write_preamble();
  print @bdf;
  print "ENDFONT\n";
}
&cleanup ();

#      +-------------------+
#      |    Subroutines    |
#      +-------------------+
sub display_glyph () {
  my ($glyphno, $bitmap) = @_;
  foreach my $p (unpack "(A4)*", $bitmap) {
    my $n = hex $p;
    foreach my $bit (map { 1 << $_ } reverse 4..15) {
      print $n & $bit ? "██" : "  ";
    }
    print "\n";
  }
}

sub convert_glyph () {
  my ($glyphno, $bitmap) = @_;
  push @bdf, "STARTCHAR U+$glyphno\n";
  push @bdf, sprintf("ENCODING %d\n", hex($glyphno));
  if (length($bitmap) == 88) {
    push @bdf, "SWIDTH 500 0\n";
    push @bdf, "DWIDTH 12 0\n";
    push @bdf, "BBX 12 22 0 -5\n";
    push @bdf, "BITMAP\n";
    foreach my $p (unpack "(A4)*", $bitmap) {
      push @bdf, "$p\n";
    }
  }
  elsif (length($bitmap) == 132) {
    push @bdf, "SWIDTH 1000 0\n";
    push @bdf, "DWIDTH 24 0\n";
    push @bdf, "BBX 24 22 0 -5\n";
    push @bdf, "BITMAP\n";
    foreach my $p (unpack "(A6)*", $bitmap) {
      push @bdf, "$p\n";
    }
  }
  else {
    printf STDERR "Unexpected bitmap length (not 88 or 132) in glyph U+%04x\n", $glyphno;
    exit 1;
  }
  push @bdf, "ENDCHAR\n";
  ++$glyphs;
}

sub write_preamble () {
  print "STARTFONT 2.1\n";
  print "FONT -sun-gallant-medium-r-normal--22-220-75-75-c-80-iso10646-1\n";
  print "SIZE 22 75 75\n";
  print "FONTBOUNDINGBOX 22 12 0 -5\n";
  print "STARTPROPERTIES 2\n";
  print "FONT_ASCENT 17\n";
  print "FONT_DESCENT 5\n";
  print "ENDPROPERTIES\n";
  print "CHARS $glyphs\n";
}

sub usage {
  print STDERR <<"EOF";
usage: $::MYNAME font.hex

  Convert a VT hex font file to X11 bdf font file.

  Options:
    --help     show this help text
    --verbose  verbose mode

  Examples:

    $::MYNAME /usr/src/share/vt/fonts/gallant.hex > gallant.bdf

EOF
  print STDERR '$Id: pl.tpl,v 1.5 2013/02/13 21:59:32 schweikh Exp schweikh $',
    "\n";
  exit 1;
}

sub initialization_and_options {
  $SIG{'INT'}  = 'cleanup';
  $SIG{'TERM'} = 'cleanup';
  $SIG{'HUP'}  = 'cleanup';
  $::MYNAME    = $0;          # Set program name for diagnostic messages.
  $::MYNAME =~ s,.*/,,;       # Strip directory part.
  $::TMP1 = "/tmp/$::MYNAME.$$";
  $::opt_help = $::opt_verbose = 0;    # to have them used at least twice
  $::opt_display = 0;
  &usage unless &GetOptions ('display', 'help', 'verbose');
  &usage if $::opt_help or $#ARGV != 0; # exactly one arg is needed
  print "$::MYNAME\n" if $::opt_verbose;
}

sub cleanup {
  unlink $::TMP1 if -f $::TMP1;
  exit;
}

# vim: syntax=perl tabstop=2 shiftwidth=2 expandtab fileformat=unix
