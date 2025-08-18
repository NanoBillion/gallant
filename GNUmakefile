.POSIX:
.SUFFIXES: .bdf .fnt .hex .src
.hex.fnt:
	vtfontcvt -o $@ $<

.hex.bdf:
	./hex2bdf.pl $< > $@

.src.hex:
	./src2hex.pl $< > $@

.PHONY: all
all:
	./src2hex.pl < gallant.src > gallant.hex
	./hex2bdf.pl gallant.hex > ~/.fonts/gallant.bdf
	cd ~/.fonts && mkfontdir && xset fp rehash
	if test $$(uname -s) = FreeBSD; then \
	  vtfontcvt -v -o gallant.fnt gallant.hex; \
	  vidcontrol -f gallant.fnt < /dev/ttyv7; \
	fi

# FreeBSD: Libs and <uniname.h> are in devel/libunistring
CC = cc -std=c99
WARNS = -Wall -Wextra
IOPTS = -I /usr/local/include
LOPTS = -L /usr/local/lib
lscp: lscp.c
	$(CC) -O3 -o $@ $(WARNS) $(IOPTS) $(LOPTS) -luninameslist -lunistring $^

txttopng: txttopng.c
	$(CC) -O3 -o $@ $(WARNS) $(IOPTS) $(LOPTS) -lpng $^

lint: txttopng.c lscp.c
	flexelint lint.lnt txttopng.c
	flexelint lint.lnt lscp.c

# X11 in x.out:
# BDF Error on line 114938: char 'U+10000' has encoding too large (65536)
